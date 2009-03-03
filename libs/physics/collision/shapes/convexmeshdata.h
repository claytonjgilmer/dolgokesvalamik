#ifndef _convexmeshdata_h_
#define _convexmeshdata_h_

#include "containers/vector.h"
#include "math/vec3.h"

struct convex_mesh_data_t
{
	vector<vec3> vert;
	vector<uint16> vertex_vertex_adjindex;
	vector<uint16> vertex_vertex_adjmap;

	vector<vec3> facenormal;

	vec3 center;
	vec3 half_extent;
};

struct mtx4x3;
void draw_hull(convex_mesh_data_t* ch, const mtx4x3& mtx);


MLINLINE void get_extremal_vertex(convex_mesh_data_t* obj, const vec3& dir, vec3& vert)
{
	f32 max_dist=dot(obj->vert[0],dir);
	uint32 max_index=0;

_fasszom:
	for (int n=obj->vertex_vertex_adjindex[max_index];n<obj->vertex_vertex_adjindex[max_index+1];++n)
	{
		f32 act_dist=dot(obj->vert[obj->vertex_vertex_adjmap[n]],dir);

		if (act_dist>max_dist)
		{
			max_dist=act_dist;
			max_index=obj->vertex_vertex_adjmap[n];
			goto _fasszom;
		}
	}

	vert=obj->vert[max_index];
}

MLINLINE void get_extremal_vertex1(convex_mesh_data_t* obj, const vec3& dir, vec3& vert)
{
	f32 max_dist=dot(obj->vert[0],dir);
	uint32 max_index=0;

	bool l=true;
	while (l)
	{
		l=false;
		for (int n=obj->vertex_vertex_adjindex[max_index];n<obj->vertex_vertex_adjindex[max_index+1];++n)
		{
			f32 act_dist=dot(obj->vert[obj->vertex_vertex_adjmap[n]],dir);

			if (act_dist>max_dist)
			{
				l=true;
				max_dist=act_dist;
				max_index=obj->vertex_vertex_adjmap[n];
			}
		}
	}

	vert=obj->vert[max_index];
}

MLINLINE void get_extremal_vertex2(convex_mesh_data_t* obj, const vec3& dir, vec3& vert)
{
	f32 max_dist=-FLT_MAX;
	uint32 max_index=-1;

	uint32 num_vert=obj->vert.size();

	for (uint32 n=0; n<num_vert; ++n)
	{
		f32 act_dist=dot(obj->vert[n],dir);

		if (act_dist>max_dist)
		{
			max_dist=act_dist;
			max_index=n;
		}
	}

	vert=obj->vert[max_index];
}

MLINLINE bool is_extremal_vertex(convex_mesh_data_t* obj, int vert_index, const vec3& dir)
{
	f32 vert_dist=dot(obj->vert[vert_index],dir);

	int adj_first=obj->vertex_vertex_adjindex[vert_index];
	int adj_last=obj->vertex_vertex_adjindex[vert_index+1];

	for (int n=adj_first; n<adj_last; ++n)
	{
		f32 act_dist=dot(obj->vert[obj->vertex_vertex_adjmap[n]],dir);

		if (act_dist>vert_dist)
			return false;
	}

	return true;
}


#endif//_convexmeshdata_h_