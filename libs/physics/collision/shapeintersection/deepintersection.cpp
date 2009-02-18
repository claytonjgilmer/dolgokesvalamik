#include "deepintersection.h"
#include "math/geometry/intersection.h"

static edge_data* get_extremal_vertex(convex_hull_t* obj, const vec3& dir, const mtx4x3& mtx)
{
	vec3 localdir=mtx.transformtransposed3x3(dir);
	return obj->get_extremal_vertex(localdir);
}


deep_intersection::deep_intersection(convex_hull_t* i_obj1, convex_hull_t* i_obj2, const mtx4x3& i_mtx1, const mtx4x3& i_mtx2, const vec3& initdir/* =to_vec3 */,edge_data* i_v1, edge_data* i_v2,int i_state):
obj1(i_obj1),
obj2(i_obj2),
mtx1(i_mtx1),
mtx2(i_mtx2)
{
	itnum=0;
	vec3 dir;
	trf_1_to_2.multiplytransposed(mtx1,mtx2);

	if (initdir.squarelength()==0)
	{
		vec3 center1(i_mtx1.transform(obj1->center));
		vec3 center2(i_mtx2.transform(obj2->center));
		dir=center2-center1;
		dir.normalize();
		v1=get_extremal_vertex(obj1,dir,i_mtx1);
		v2=get_extremal_vertex(obj2,-dir,i_mtx2);

		if (dot(dir,mtx1.transform(obj1->get_vertexpos(v1))-mtx2.transform(obj2->get_vertexpos(v2)))<=0)
		{
			result=false;
			return;
		}
	}
	else
	{
		dir=initdir;
		v1=obj1->get_extremal_vertex(mtx1.transformtransposed3x3(dir),i_v1);
		v2=obj2->get_extremal_vertex(mtx2.transformtransposed3x3(-dir),i_v2);
//		state=i_state;
		depth=dot(dir,mtx2.transform(obj2->get_vertexpos(v2))-mtx1.transform(obj1->get_vertexpos(v1)));

		if (depth>=0)
		{
			result=false;
			return;
		}
	}

	depth=-FLT_MAX;
	report_min_pair(v1,v2);//, state, depth,v1,v2);

	bool ret=false;
	do
	{
		if (depth>0)
		{
			result=false;
			return;
		}

//		edge_data* s11,*s12,*s21,*s22;
//		edge_data* s1,*s2;
//		float depth1,depth2;
//		int state1,state2;
		switch(state)
		{
		case edge_edge:
			{
#if 0
				if (!(ret=report_min_pair(v1,obj2->get_opposite(v2))))
					ret=report_min_pair(obj1->get_opposite(v1),v2);
#else
				if (!(ret=report_min_pair_face(v1,obj2->get_opposite(v2))))
					if (!(ret=report_min_pair_face(obj1->get_opposite(v1),v2)))
						if (!(ret=report_min_pair_edge(v1,obj2->get_opposite(v2))))
							ret=report_min_pair_edge(obj1->get_opposite(v1),v2);
#endif

				break;
			}
		case face_vertex:
			{
#if 0
				if (!(ret=report_min_pair(obj1->get_next(v1),v2)))
					ret=report_min_pair(obj1->get_prev(v1),v2);
#else
				if (!(ret=report_min_pair_face(obj1->get_next(v1),v2)))
					if (!(ret=report_min_pair_face(obj1->get_prev(v1),v2)))
						if (!(ret=report_min_pair_edge(obj1->get_next(v1),v2)))
							ret=report_min_pair_edge(obj1->get_prev(v1),v2);
#endif
				break;

			}
		case vertex_face:
			{
#if 0
				if (!(ret=report_min_pair(v1,obj2->get_next(v2))))
					ret=report_min_pair(v1,obj2->get_prev(v2));
#else
				if (!(ret=report_min_pair_face(v1,obj2->get_next(v2))))
					if (!(ret=report_min_pair_face(v1,obj2->get_prev(v2))))
						if (!(ret=report_min_pair_edge(v1,obj2->get_next(v2))))
							ret=report_min_pair_edge(v1,obj2->get_prev(v2));
#endif
				break;
			}
		default:
			assertion(0);
		}

		++itnum;
	}while (ret);

	switch (state)
	{
	case face_vertex:
		normal=mtx2.transform3x3(normal)*depth;
		break;
	case vertex_face:
	case edge_edge:
		normal=mtx1.transform3x3(normal)*(-depth);
		break;
	}

	result=(depth<=0);
}

bool deep_intersection::report_min_pair(edge_data* i_v1, edge_data* i_v2)//, int& o_state, float& o_depth,edge_data*& o_s1, edge_data*& o_s2)
{
	edge_data* e;
	bool ret=false;

//	o_depth=-FLT_MAX;

	vec3 vert1=trf_1_to_2.transform(obj1->get_vertexpos(i_v1));
	vec3 vert2=obj2->get_vertexpos(i_v2);
	vec3 dir12=vert1-vert2;

	/*
	vegigmegyunk i_v1 osszes szomszedos face-en, es ha valamelyik normalja iranyaban i_v2 extremalis vertex, akkor ott tavolsagot nezunk
	valamint kigyujtjuk azokat az eleket, amiket majd figyelni kell
	*/
	e=i_v1;
	do 
	{
		vec3 facenormal=trf_1_to_2.transform3x3(-obj1->get_face(e)->normal);
		if (obj2->is_extremal_vertex(facenormal,i_v2)) //ha a face belul van a poligonon, akkor megnezzuk, hogy arra milyen messze van a kifele
		{
			float dist=dot(facenormal,dir12);

			if (dist>depth+.0001)
			{
				normal=facenormal;
				depth=dist;
				state=face_vertex;
				v1=e;
				v2=i_v2;
				ret=true;
			}
		}
		e=obj1->get_rot_edge(e);
	} while (e!=i_v1);
	/*
	vegigmegyunk i_v2 osszes szomszedos face-en, es ha valamelyik normalja iranyaban i_v1 extremalis vertex, akkor ott tavolsagot nezunk
	valamint kigyujtjuk azokat az eleket, amiket majd figyelni kell
	*/
	e=i_v2;
	dir12=-trf_1_to_2.transformtransposed3x3(dir12);
	do 
	{
		vec3 facenormal=trf_1_to_2.transformtransposed3x3(-obj2->get_face(e)->normal);

		if (obj1->is_extremal_vertex(facenormal,i_v1))
		{
			float dist=dot(facenormal,dir12);

			if (dist>depth+.0001)
			{
				normal=facenormal;
				depth=dist;
				state=vertex_face;
				v1=i_v1;
				v2=e;
				ret=true;
			}
		}
		e=obj2->get_rot_edge(e);
	} while (e!=i_v2);

//	return false;
	e=i_v1;

	do 
	{
		edge_data* e2=i_v2;
		do 
		{
			vec3 edge_edge_normal;
			if (spherical_segment_intersect(edge_edge_normal,obj1->get_face_normal(e),obj1->get_opposite_face_normal(e),
															 -trf_1_to_2.transformtransposed3x3(obj2->get_face_normal(e2)),-trf_1_to_2.transformtransposed3x3(obj2->get_opposite_face_normal(e2))))
			{
				float dist=dot(edge_edge_normal,trf_1_to_2.transformtransposed(obj2->get_vertexpos(e2))-obj1->get_vertexpos(e));

				if (dist>depth+.0001)
				{
					normal=edge_edge_normal;
					depth=dist;
					state=edge_edge;
					v1=e;
					v2=e2;
					ret=true;
				}
			}
			e2=obj2->get_rot_edge(e2);
		} while (e2!=i_v2);
		e=obj1->get_rot_edge(e);
	} while (e!=i_v1);

	return ret;
}


bool deep_intersection::report_min_pair_face(edge_data* i_v1, edge_data* i_v2)//, int& o_state, float& o_depth,edge_data*& o_s1, edge_data*& o_s2)
{
	edge_data* e;

	vec3 vert1=trf_1_to_2.transform(obj1->get_vertexpos(i_v1));
	vec3 vert2=obj2->get_vertexpos(i_v2);
	vec3 dir12=vert1-vert2;

	/*
	vegigmegyunk i_v1 osszes szomszedos face-en, es ha valamelyik normalja iranyaban i_v2 extremalis vertex, akkor ott tavolsagot nezunk
	valamint kigyujtjuk azokat az eleket, amiket majd figyelni kell
	*/
	e=i_v1;
	do 
	{
		vec3 facenormal=trf_1_to_2.transform3x3(-obj1->get_face(e)->normal);
		if (obj2->is_extremal_vertex(facenormal,i_v2)) //ha a face belul van a poligonon, akkor megnezzuk, hogy arra milyen messze van a kifele
		{
			float dist=dot(facenormal,dir12);

			if (dist>depth+.0001)
			{
				normal=facenormal;
				depth=dist;
				state=face_vertex;
				v1=e;
				v2=i_v2;
				return true;
			}
		}
		e=obj1->get_rot_edge(e);
	} while (e!=i_v1);
	/*
	vegigmegyunk i_v2 osszes szomszedos face-en, es ha valamelyik normalja iranyaban i_v1 extremalis vertex, akkor ott tavolsagot nezunk
	valamint kigyujtjuk azokat az eleket, amiket majd figyelni kell
	*/
	e=i_v2;
	dir12=-trf_1_to_2.transformtransposed3x3(dir12);
	do 
	{
		vec3 facenormal=trf_1_to_2.transformtransposed3x3(-obj2->get_face(e)->normal);

		if (obj1->is_extremal_vertex(facenormal,i_v1))
		{
			float dist=dot(facenormal,dir12);

			if (dist>depth+.0001)
			{
				normal=facenormal;
				depth=dist;
				state=vertex_face;
				v1=i_v1;
				v2=e;
				return true;
			}
		}
		e=obj2->get_rot_edge(e);
	} while (e!=i_v2);

	return false;
}

bool deep_intersection::report_min_pair_edge(edge_data* i_v1, edge_data* i_v2)
{
	edge_data* e;
	e=i_v1;

	do 
	{
		edge_data* e2=i_v2;
		do 
		{
			vec3 edge_edge_normal;
			if (spherical_segment_intersect(edge_edge_normal,obj1->get_face_normal(e),obj1->get_opposite_face_normal(e),
				-trf_1_to_2.transformtransposed3x3(obj2->get_face_normal(e2)),-trf_1_to_2.transformtransposed3x3(obj2->get_opposite_face_normal(e2))))
			{
				float dist=dot(edge_edge_normal,trf_1_to_2.transformtransposed(obj2->get_vertexpos(e2))-obj1->get_vertexpos(e));

				if (dist>depth+.0001)
				{
					normal=edge_edge_normal;
					depth=dist;
					state=edge_edge;
					v1=e;
					v2=e2;
					return true;
				}
			}
			e2=obj2->get_rot_edge(e2);
		} while (e2!=i_v2);
		e=obj1->get_rot_edge(e);
	} while (e!=i_v1);

	return false;
}
