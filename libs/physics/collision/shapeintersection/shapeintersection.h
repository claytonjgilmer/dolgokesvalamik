#ifndef _shapeintersection_h_
#define _shapeintersection_h_

#include "physics/collision/shapes/shape.h"
#include <float.h>

MLINLINE int test_sphere_sphere_intersect(shape_t* i_sph1, const mtx4x3& i_body1_mtx,
                                          shape_t* i_sph2, const mtx4x3& i_body2_mtx,
                                          vec3 o_contact_array[][2],
                                          vec3& o_normal,
                                          uint32& o_contact_num)
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
        o_contact_array[0][0]=i_body1_mtx.transformtransposed(sph1_pos+sph1->radius*dir);
        o_contact_array[0][1]=i_body2_mtx.transformtransposed(sph2_pos-sph2->radius*dir);

        return 1;
    }
    return 0;
}


//#error gondoljuk csak at eztet meg egyszer
MLINLINE int test_sphere_box_intersect(shape_t* i_shape1, const mtx4x3& i_body1_mtx,
                                       shape_t* i_shape2, const mtx4x3& i_body2_mtx,
                                       vec3 o_contact_array[][2],
                                       vec3& o_normal,
                                       uint32& o_contact_num)
{
	assertion(i_shape1->type=shape_type_sphere && i_shape2->type==shape_type_box);

	box_shape* box=(box_shape*)i_shape2;
	sphere_shape* sphere=(sphere_shape*)i_shape1;

	const mtx4x3& sphere_mtx=i_body1_mtx;
	mtx4x3 box_mtx; box_mtx.multiply(box->pos,i_body2_mtx);


    vec3 box_extent=box->extent;
    vec3 sphere_abs_center; sphere_mtx.transform(sphere_abs_center,sphere->center);
	vec3 sphere_rel_center;
    box_mtx.transformtransposed(sphere_rel_center,sphere_abs_center);
    f32 sphere_radius=sphere->radius;

    vec3 penetration;



    for (int n=0; n<3; ++n)
    {
        if (sphere_rel_center[n]>box_extent[n])
            penetration[n]=box_extent[n]-sphere_rel_center[n];
        else if (sphere_rel_center[n]<-box_extent[n])
            penetration[n]=-box_extent[n]-sphere_rel_center[n];
        else
            penetration[n]=0;
    }

    f32 dist2=penetration.squarelength();

    if (dist2>sphere_radius*sphere_radius)
        return 0;

    //most mar tuti utkozunk
    //ha dist2>0, akkor a kozeppont kivul van

    o_contact_num=1;

	//a box koordinatarendszereben vagyunk

    if (dist2>0)
    {
		penetration.normalize();
        box_mtx.transform3x3(o_normal,penetration);
		vec3 contact_sphere_world=box_mtx.transform(sphere_rel_center-penetration);

		o_contact_array[0][0]=sphere_mtx.transformtransposed(contact_sphere_world);
		o_contact_array[0][1]=sphere_rel_center-sphere_radius*penetration;
    }
    else
    {
        //a sphere kozeppontja belul van a boxon, meg kell keresni a legrovidebb kiutat
        f32 dist=FLT_MAX;
        int index;
        for (int n=0; n<3; ++n)
        {
            f32 d;
            d=box_extent[n]-fabsf(sphere_rel_center[n]);
            if (d<dist)
            {
                index=n;
                dist=d;
            }
        }

        penetration[index]=dist*sign(sphere_rel_center[index]);
        o_normal=penetration;
        o_normal.normalize();

        o_contact_array[0][0]=sphere_center+penetration;
        o_contact_array[0][1]=sphere_center+sphere_radius*o_normal;
    }
    return 1;
}

#endif//_shapeintersection_h_
