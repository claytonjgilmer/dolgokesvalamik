#ifndef _satintersection_h_
#define _satintersection_h_

#include "physics/collision/shapes/shape.h"

struct convex_mesh_data_t
{

};

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