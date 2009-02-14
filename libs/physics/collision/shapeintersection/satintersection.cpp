#include "satintersection.h"
#include "math/plane.h"
#include "render/rendersystem.h"


void partial_hull::draw(const vector<vec3>& vert_array, const mtx4x3& mtx)
{
	for (unsigned n=0; n<num_edges; ++n)
	{
		vec3 start=mtx.transform(vert_array[edgeverts[2*n]]);
		vec3 end=mtx.transform(vert_array[edgeverts[2*n+1]]);
		rendersystem::ptr->draw_line(start,color_r8g8b8a8(255,0,0,255),end,color_r8g8b8a8(255,0,0,255));
	}
}

bool test_sat_intersect(convex_mesh_data_t* i_data1, const mtx4x3& i_mtx1,
						convex_mesh_data_t* i_data2, const mtx4x3& i_mtx2,
						vec3 o_contact_array[][2],
						vec3& o_normal,
						uint32& o_contact_num)
{
	
	return false;
}

bool test_sat_intersect(shape_t* i_sph1, const mtx4x3& i_body1_mtx,
						shape_t* i_sph2, const mtx4x3& i_body2_mtx,
						vec3 o_contact_array[][2],
						vec3& o_normal,
						uint32& o_contact_num)
{
	convex_mesh_shape_t* shape1=(convex_mesh_shape_t*)i_sph1;
	convex_mesh_shape_t* shape2=(convex_mesh_shape_t*)i_sph2;

	return test_sat_intersect(shape1->data,i_body1_mtx,shape2->data,i_body2_mtx,o_contact_array,o_normal,o_contact_num);
}









void cull(partial_hull& hull, const convex_mesh_data_t* obj, const plane_t& plane)
{
	hull.num_verts=0;
	unsigned vert_count=obj->vert.size();
	char* culledvertex=(char*)_alloca(vert_count);
	memset(culledvertex,0,vert_count);

	//elvagjuk a vertexeket
	for (unsigned n=0; n<vert_count; ++n)
	{
		float dist=plane.get_distance(obj->vert[n]);

		if (dist>0)
		{
			culledvertex[n]=1;
			hull.add_vertex(n);
		}
	}

	assertion(hull.num_verts);

	hull.num_edges=0;
	hull.num_faces=0;


	//most elvagjuk a face-eket es az eleket
	unsigned face_count=obj->facevertindex.size()-1;
	for (unsigned n=0; n<face_count; ++n)
	{
		if (dot(plane.normal,obj->facenormal[n])<0)
			continue;

		int firstindex=obj->facevertindex[n];
		int lastindex=obj->facevertindex[n+1];

		int previndex=obj->facevert[lastindex-1];
		int culledface=0;
		int actindex;

		for (int m=firstindex; m<lastindex; ++m,previndex=actindex)
		{
			actindex=obj->facevert[m];
			if (culledvertex[previndex]!=1 && culledvertex[actindex]!=1)
				continue;

			if (!culledvertex[previndex])
			{
				culledvertex[previndex]=2;
				hull.add_vertex(previndex);
			}
			if (!culledvertex[actindex])
			{
				culledvertex[actindex]=2;
				hull.add_vertex(actindex);
			}

			vec3 dir=obj->vert[previndex]-obj->vert[actindex]; dir.normalize();

			int i;
			for (i=0; i<hull.num_edges; ++i)
			{
				if (fabsf(dot(hull.edges[i],dir))>.9999f)
				{
					strlen("");
					goto _faszom;
				}
			}

			hull.add_edge(dir,previndex,actindex);
_faszom:
			culledface=1;
		}

		if (culledface)
			hull.add_face(n);
	}
}

void get_extremal_vertex(const partial_hull& hull, const convex_mesh_data_t* obj, const vec3& dir, float& dist)
{
	for (unsigned n=0; n<hull.num_verts; ++n)
	{
		float act_dist=dot(dir,obj->vert[hull.verts[n]]);

		if (act_dist>dist)
			dist=act_dist;
	}
}

void get_extremal_vertex(const partial_hull& hull, const convex_mesh_data_t* obj, const vec3& dir, vec3& max_vert)
{
	float dist=-FLT_MAX;
	unsigned max_index=-1;
	for (unsigned n=0; n<hull.num_verts; ++n)
	{
		unsigned act_index=hull.verts[n];
		float act_dist=dot(dir,obj->vert[act_index]);

		if (act_dist>dist)
		{
			dist=act_dist;
			max_index=act_index;
		}
	}

	max_vert=obj->vert[max_index];
}

bool test_sat_intersection(convex_mesh_data_t* i_data1, const mtx4x3& i_mtx1,
						   convex_mesh_data_t* i_data2, const mtx4x3& i_mtx2,
						   vec3& o_normal, float& o_depth)
{
	/*
	a lenyeg az hogy ne kelljen vegigmenni az osszes elen
	tfh minden el kap egy azonositot, hogy dupla el -e, es csak akkor nem rakjuk rogton a vegere, ha az
	ha 
	*/

	/*
	mi lenne, ha nem a face-eken mennenk vegig, hanem az eleken
	igy minden el csak 1x lenne feldolgozva
	viszont nem tudjuk, hogy mely face-eket kell hozzaadni, csak ha van adjacency informacionk
	*/

	/*
	eloszor csinaljuk meg az alapesetet
	*/

	mtx4x3 trf_1_to_2; trf_1_to_2.multiplytransposed(i_mtx1,i_mtx2);

	vec3 dir1=trf_1_to_2.transformtransposed(i_data2->center)-i_data1->center;
	vec3 dir2=trf_1_to_2.transform(i_data1->center)-i_data2->center;

	vec3 max1,max2;
	get_extremal_vertex(i_data1,dir1,max1);
	get_extremal_vertex(i_data2,dir2,max2);

	float max_dist=dot(dir1,trf_1_to_2.transformtransposed(max2)-max1);

	if (max_dist>=0)
		return false;

	plane_t plane1(trf_1_to_2.transformtransposed(max2),dir1),plane2(trf_1_to_2.transform(max1),dir2);

	partial_hull hull1, hull2;
	cull(hull1,i_data1,plane1);
	cull(hull2,i_data2,plane2);
//	i_data1->ph=hull1;
//	i_data2->ph=hull2;
	return false;

	max_dist=-FLT_MAX;
	vec3 sep_axis;
	vec3* face_normal=&i_data1->facenormal[0];
	for (unsigned n=0; n<hull1.num_faces; ++n)
	{
		vec3 normal=-(trf_1_to_2.transform3x3(face_normal[hull1.faces[n]]));
		vec3 v;
		get_extremal_vertex(hull2,i_data2,normal,v);
		vec3 v2=i_data1->vert[i_data1->facevert[i_data1->facevertindex[n]]];
		float dist=dot(face_normal[hull1.faces[n]],trf_1_to_2.transformtransposed(v)-v2);

		if (dist>0)
			return false;

		if (dist>max_dist)
		{
			max_dist=dist;
			sep_axis=face_normal[hull1.faces[n]];
		}
	}

	face_normal=&i_data2->facenormal[0];
	for (unsigned n=0; n<hull2.num_faces; ++n)
	{
		vec3 normal=-(trf_1_to_2.transformtransposed3x3(face_normal[hull2.faces[n]]));
		vec3 v;
		get_extremal_vertex(hull1,i_data1,normal,v);
		float dist=dot(face_normal[hull2.faces[n]],trf_1_to_2.transform(v)-i_data2->vert[i_data2->facevert[i_data2->facevertindex[n]]]);

		if (dist>0)
			return false;

		if (dist>max_dist)
		{
			max_dist=dist;
			sep_axis=face_normal[hull2.faces[n]];
		}
	}

	for (unsigned n=0; n<hull1.num_edges; ++n)
	{
		for (unsigned m=0; m<hull2.num_edges; ++m)
		{
			vec3 dir1; dir1.cross(hull1.edges[n],trf_1_to_2.transformtransposed3x3(hull2.edges[m]));
			vec3 dir2=trf_1_to_2.transform3x3(dir1);

			if (dot(plane1.normal,dir1)<0)
				dir1=-dir1;
			else
				dir2=-dir2;

			vec3 max_vert1, max_vert2;
			get_extremal_vertex(hull1,i_data1,dir1,max_vert1);
			get_extremal_vertex(hull2,i_data2,dir2,max_vert2);

			dir1.normalize();
			float dist=dot(dir1,trf_1_to_2.transformtransposed(max_vert2)-max_vert1);

			if (dist>0)
				return false;

			if (dist>max_dist)
			{
				max_dist=dist;
				sep_axis=dir1;
			}
		}
	}

	o_normal=sep_axis;
	o_depth=-max_dist;
	return true;
}

void draw_hull(convex_mesh_data_t* ch, const mtx4x3& mtx)
{
	for (int n=0; n<(int)ch->vertex_vertex_adjindex.size()-1; ++n)
	{
		int firstadj=ch->vertex_vertex_adjindex[n];
		int adjnum=ch->vertex_vertex_adjindex[n+1]-firstadj;

		vec3 start=mtx.transform(ch->vert[n]);

		for (int m=0; m<adjnum; ++m)
		{
			int vindex=ch->vertex_vertex_adjmap[firstadj+m];

			if (vindex<n)
			{
				vec3 end=mtx.transform(ch->vert[vindex]);
				rendersystem::ptr->draw_line(start,color_r8g8b8a8(255,255,255,255),end,color_r8g8b8a8(255,255,255,255));
			}
		}
	}
}

