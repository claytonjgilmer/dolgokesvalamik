#ifndef _shapeintersection_h_
#define _shapeintersection_h_

#include "physics/collision/shapes/shape.h"

	MLINLINE int test_sphere_sphere_intersect(shape_t* i_sph1, const mtx4x3& i_body1_mtx, shape_t* i_sph2, const mtx4x3& i_body2_mtx, vec3 o_contact_array[][2], vec3& o_normal, uint32& o_contact_num)
	{
		sphere_shape* sph1=(sphere_shape*)i_sph1;
		sphere_shape* sph2=(sphere_shape*)i_sph2;
		vec3 sph1_pos=i_body1_mtx.transform(sph1->center);
		vec3 sph2_pos=i_body2_mtx.transform(sph2->center);

		vec3 dir=sph2_pos-sph1_pos;

		if (dir.squarelength()<sqr(sph1->radius+sph2->radius))
		{
			o_contact_num=1;
			dir.normalize();
			o_normal=dir;
			o_contact_array[0][1]=sph1_pos+sph1->radius*dir;
			o_contact_array[0][2]=sph2_pos-sph2->radius*dir;

			return 1;
		}
		return 0;
	}
#endif//_shapeintersection_h_