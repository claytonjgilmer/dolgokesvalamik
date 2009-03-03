#ifndef _satintersection_h_
#define _satintersection_h_
#if 0
#include "physics/collision/shapes/shape.h"
#include "physics/collision/shapes/convexmeshdata.h"
#include "containers/vector.h"

#define MAX_FEATURE_COUNT 512
struct partial_hull
{
	uint16 verts[MAX_FEATURE_COUNT];
	uint16 faces[MAX_FEATURE_COUNT];
	vec3 edges[MAX_FEATURE_COUNT];

	uint16 edgeverts[MAX_FEATURE_COUNT*2];

	uint16 num_verts;
	uint16 num_edges;
	uint16 num_faces;

	__forceinline void add_vertex(const uint16 v)
	{
		verts[num_verts++]=v;
		assertion(num_verts<=MAX_FEATURE_COUNT);
	}
	__forceinline void add_face(const uint16 f)
	{
		faces[num_faces++]=f;
		assertion(num_faces<=MAX_FEATURE_COUNT);
	}
	__forceinline void add_edge(const vec3& e, uint16 index1, uint16 index2)
	{
		edgeverts[2*num_edges]=index1;
		edgeverts[2*num_edges+1]=index2;
		edges[num_edges++]=e;
		assertion(num_edges<=MAX_FEATURE_COUNT);
	}

	void draw(const vector<vec3>& vert_array, const mtx4x3& mtx);
};



bool test_sat_intersection(convex_mesh_data_t* i_data1, const mtx4x3& i_mtx1,
						   convex_mesh_data_t* i_data2, const mtx4x3& i_mtx2,
						   vec3& o_normal, f32& o_depth);

struct convex_mesh_shape_t:shape_t
{
	convex_mesh_data_t* data;
};


bool test_sat_intersect(shape_t* i_sph1, const mtx4x3& i_body1_mtx,
						shape_t* i_sph2, const mtx4x3& i_body2_mtx,
						vec3 o_contact_array[][2],
						vec3& o_normal,
						uint32& o_contact_num);

#endif
#endif//_satintersection_h_