#include "satintersection.h"

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









void get_max_vert(convex_mesh_data_t* obj, const vec3& dir, vec3& vert, float& dist)
{
	float max_dist=dot(obj->vert[0],dir);
	uint32 max_index=0;
	bool tovabb=true;

	while (tovabb)
	{
		tovabb=false;
		for (int n=obj->adjindex[max_index];n<obj->adjindex[max_index+1];++n)
		{
			float act_dist=dot(obj->vert[obj->adjmap[n]],dir);

			if (act_dist>max_dist)
			{
				max_dist=act_dist;
				max_index=obj->adjmap[n];
				tovabb=true;
				break;
			}
		}
	}

	dist=max_dist;
	vert=obj->vert[max_index];
}

void get_max_vert2(convex_mesh_data_t* obj, const vec3& dir, vec3& vert, float& dist)
{
	float max_dist=-FLT_MAX;
	uint32 max_index=-1;

	uint32 num_vert=obj->vert.size()-1;

	for (uint32 n=0; n<num_vert; ++n)
	{
		float act_dist=dot(obj->vert[n],dir);

		if (act_dist>max_dist)
		{
			max_dist=act_dist;
			max_index=n;
		}
	}

	dist=max_dist;
	vert=obj->vert[max_index];
}

bool is_extremal_vertex(convex_mesh_data_t* obj, int vert_index, const vec3& dir)
{
	float vert_dist=dot(obj->vert[vert_index],dir);

	int adj_first=obj->adjindex[vert_index];
	int adj_last=obj->adjindex[vert_index+1];

	for (int n=adj_first; n<adj_last; ++n)
	{
		float act_dist=dot(obj->vert[obj->adjmap[n]],dir);

		if (act_dist>vert_dist)
			return false;
	}

	return true;
}


struct partial_hull
{
	uint16 face[32];
	int num_faces;
	uint16 vert[32];
	int num_verts;
	uint16 edge[32];
	int num_edges;
};


bool test_sat_intersection(convex_mesh_data_t* i_data1, const mtx4x3& i_mtx1,
						   convex_mesh_data_t* i_data2, const mtx4x3& i_mtx2,
						   vec3& o_normal, float& o_depth)
{
	return false;
}
