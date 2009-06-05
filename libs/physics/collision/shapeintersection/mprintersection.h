#ifndef _mprintersection_h_
#define _mprintersection_h_

struct convex_mesh_data_t;
#include "math/dvec3.h"
#include "math/mtx4x3.h"

struct mpr_intersection
{
	mpr_intersection(convex_mesh_data_t* obj1, convex_mesh_data_t* obj2,const mtx4x3& mtx1, const mtx4x3& mtx2);

	int iteration;
	bool result;

	vec3 point1;
	vec3 point2;
	vec3 normal;

	bool test_intersection(convex_mesh_data_t* p1, const mtx4x3& m1, convex_mesh_data_t* p2, const mtx4x3& m2, vec3& returnNormal, vec3& point1, vec3& point2);

};
#endif//_mprintersection_h_