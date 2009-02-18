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

MLINLINE int test_sphere_box_intersect(shape_t* i_shape1, const mtx4x3& i_body1_mtx,
                                       shape_t* i_shape2, const mtx4x3& i_body2_mtx,
                                       vec3 o_contact_array[][2],
                                       vec3& o_normal,
                                       uint32& o_contact_num)
{
    int flip;
    vec3 box_extent;
    vec3 sphere_center;
    float sphere_radius;
    if (i_shape1->type==shape_type_box)
    {
        mtx4x3 box_mtx;
        flip=TRUE;
        box_extent=((box_shape*)i_shape1)->extent;
        box_mtx.multiply(((box_shape*)i_shape1)->pos,i_body1_mtx);
        vec3 sphere_abs_center; i_body2_mtx.transform(sphere_abs_center,((sphere_shape*)i_shape2)->center);
        box_mtx.transformtransposed(sphere_center,sphere_abs_center);
        sphere_radius=((sphere_shape*)i_shape2)->radius;
    }
    else
    {
        mtx4x3 box_mtx;
        flip=FALSE;
        box_extent=((box_shape*)i_shape2)->extent;
        box_mtx.multiply(((box_shape*)i_shape2)->pos,i_body2_mtx);
        vec3 sphere_abs_center; i_body1_mtx.transform(sphere_abs_center,((sphere_shape*)i_shape1)->center);
        box_mtx.transformtransposed(sphere_center,sphere_abs_center);
        sphere_radius=((sphere_shape*)i_shape1)->radius;
    }

    vec3 penetration;



    for (int n=0; n<3; ++n)
    {
        if (sphere_center[n]>box_extent[n])
            penetration[n]=sphere_center[n]-box_extent[n];
        else if (sphere_center[n]<-box_extent[n])
            penetration[n]=sphere_center[n]+box_extent[n];
        else
            penetration[n]=0;
    }

    float dist2=penetration.squarelength();

    if (dist2>sphere_radius*sphere_radius)
        return 0;

    //most mar tuti utkozunk
    //ha dist2>0, akkor a kozeppont kivul van

    o_contact_num=1;

    if (dist2>0)
    {
        o_normal=penetration;
        o_normal.normalize();
        o_contact_array[0][0]=sphere_center-penetration;
        o_contact_array[0][1]=sphere_center-sphere_radius*o_normal;

    }
    else
    {
        //a sphere kozeppontja belul van a boxon, meg kell keresni a legrovidebb kiutat
        float dist=FLT_MAX;
        int index;
        for (int n=0; n<3; ++n)
        {
            float d;
            d=box_extent[n]-fabsf(sphere_center[n]);
            if (d<dist)
            {
                index=n;
                dist=d;
            }
        }

        penetration[index]=dist;
        o_normal=penetration;
        o_normal.normalize();
        o_contact_array[0][0]=sphere_center+penetration;
        o_contact_array[0][1]=sphere_center+sphere_radius*o_normal;
    }

    if (flip)
    {
        o_normal=-o_normal;
        swap(o_contact_array[0][0],o_contact_array[0][1]);
    }
    return 1;
}

#endif//_shapeintersection_h_
