#ifndef _satintersection_h_
#define _satintersection_h_

#include "physics/collision/shapes/shape.h"
#include "containers/vector.h"

struct convex_mesh_data_t
{
	vector<vec3> vert;
	vector<uint16> adjindex;
	vector<uint16> adjmap;

	vector<vec3> facenormal;
	vector<uint16> facevertindex;
	vector<uint16> facevert;

	vec3 center;
	vec3 half_extent;
};

bool test_sat_intersection(convex_mesh_data_t* i_data1, const mtx4x3& i_mtx1,
						   convex_mesh_data_t* i_data2, const mtx4x3& i_mtx2,
						   vec3& o_normal, float& o_depth);

struct convex_mesh_shape_t:shape_t
{
	convex_mesh_data_t* data;
};


bool test_sat_intersect(shape_t* i_sph1, const mtx4x3& i_body1_mtx,
						shape_t* i_sph2, const mtx4x3& i_body2_mtx,
						vec3 o_contact_array[][2],
						vec3& o_normal,
						uint32& o_contact_num);

#endif//_satintersection_h_