#include "satintersection.h"

bool test_sat_intersect(convex_mesh_data_t* i_data1, const mtx4x3& i_mtx2,
						shape_t* i_data2, const mtx4x3& i_mtx1,
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

	return test_sat_intersect(shape1->data,shape2->data,i_body1_mtx,i_body2_mtx,o_contact_array,o_normal,o_contact_num);
}