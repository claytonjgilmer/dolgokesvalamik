#ifndef _shapeintersection_h_
#define _shapeintersection_h_

#include "physics/collision/shapes/shape.h"
#include "math/geometry/intersection.h"
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


//#error gondoljuk csak at eztet meg egyszer hirtelen jol
MLINLINE int test_sphere_box_intersect(shape_t* i_shape1, const mtx4x3& i_body1_mtx,
                                       shape_t* i_shape2, const mtx4x3& i_body2_mtx,
                                       vec3 o_contact_array[][2],
                                       vec3& o_normal,
                                       uint32& o_contact_num)
{
	assertion(i_shape1->type==shape_type_sphere && i_shape2->type==shape_type_box);

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
        box_mtx.transform3x3(o_normal,penetration);
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
        box_mtx.transform3x3(o_normal,-penetration);
    }
	o_normal.normalize();
	vec3 contact_sphere_world=box_mtx.transform(sphere_rel_center)+sphere_radius*o_normal;
	o_contact_array[0][0]=sphere_mtx.transformtransposed(contact_sphere_world); //a gomb koordinatarendszereben
	o_contact_array[0][1]=box->pos.transform(sphere_rel_center+penetration); // a box kooordinatarendszereben
    return 1;
}

MLINLINE int test_box_sphere_intersect(shape_t* i_shape1, const mtx4x3& i_body1_mtx,
									   shape_t* i_shape2, const mtx4x3& i_body2_mtx,
									   vec3 o_contact_array[][2],
									   vec3& o_normal,
									   uint32& o_contact_num)
{
	assertion(i_shape1->type==shape_type_box && i_shape2->type==shape_type_sphere);

	int ret=test_sphere_box_intersect(i_shape2,i_body2_mtx,i_shape1,i_body1_mtx,o_contact_array,o_normal,o_contact_num);

	if (ret)
	{
		o_normal=-o_normal;
		swap(o_contact_array[0][0],o_contact_array[0][1]);
	}

	return ret;
}


#define BOX_EPSILON .0001f
#define BOX_CUTOFF (1.0f-BOX_EPSILON)
MLINLINE int test_box_box_intersect(shape_t* i_shape1, const mtx4x3& i_body1_mtx,
									shape_t* i_shape2, const mtx4x3& i_body2_mtx,
									vec3 o_contact_array[][2],
									vec3& o_normal,
									uint32& o_contact_num)
{
	assertion(i_shape1->type==shape_type_box && i_shape2->type==shape_type_box);
	box_shape* box1=(box_shape*)i_shape1;
	box_shape* box2=(box_shape*)i_shape2;

	mtx4x3 box1_mtx; box1_mtx.multiply(box1->pos,i_body1_mtx);
	mtx4x3 box2_mtx; box2_mtx.multiply(box2->pos,i_body2_mtx);

	mtx3x3 R; R.multiplytransposed3x3(box2_mtx,box1_mtx);

	float ra, rb;
	mtx3x3 abs_R;

	// Compute translation vector t
	// Bring translation into a's coordinate frame
	vec3 t;  box1_mtx.transformtransposed3x3(t,box2_mtx.t - box1_mtx.t);

	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	bool parallel_edge_pair=false;


	float penetration=-FLT_MAX;
	int collision_code;
	const vec3& e1=box1->extent;
	const vec3& e2=box2->extent;
	float dist;

	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			abs_R.axis(j)[i] = fabs(R.axis(j)[i]);
			if (abs_R.axis(j)[i]>BOX_CUTOFF)
				parallel_edge_pair=true;
		}
		ra = e1[i];
		rb = e2.x * abs_R.axis(0)[i] +
			e2.y * abs_R.axis(1)[i] +
			e2.z * abs_R.axis(2)[i];

		dist=fabs(t[i])-(ra + rb);
		if (dist>0)
			return 0;

		if (dist>penetration)
		{
			penetration=dist;
			collision_code=i;
		}
	}

	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++)
	{
		ra=	e1.x*abs_R.axis(i).x+
			e1.y*abs_R.axis(i).y+
			e1.z*abs_R.axis(i).z;
		rb = e2[i];

		dist=fabs(dot(t,R.axis(i))) -(ra + rb);
		if (dist>0) return 0;

		if (dist>penetration)
		{
			collision_code=3+i;
			penetration=dist;
		}
	}

	if (!parallel_edge_pair)
	{
		// Test axis L = A0 x B0
		ra = e1[1] * abs_R.axis(0)[2] + e1[2] * abs_R.axis(0)[1];
		rb = e2[1] * abs_R.axis(2)[0] + e2[2] * abs_R.axis(1)[0];
		dist=fabs(t[2] * R.axis(0)[1] - t[1] * R.axis(0)[2]) -(ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			collision_code=6;
		}

		// Test axis L = A0 x B1
		ra = e1[1] * abs_R.axis(1)[2] + e1[2] * abs_R.axis(1)[1];
		rb = e2[0] * abs_R.axis(2)[0] + e2[2] * abs_R.axis(0)[0];
		dist=fabs(t[2] * R.axis(1)[1] - t[1] * R.axis(1)[2]) -( ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			collision_code=7;
		}

		// Test axis L = A0 x B2
		ra = e1[1] * abs_R.axis(2)[2] + e1[2] * abs_R.axis(2)[1];
		rb = e2[0] * abs_R.axis(1)[0] + e2[1] * abs_R.axis(0)[0];
		dist=fabs(t[2] * R.axis(2)[1] - t[1] * R.axis(2)[2]) - (ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			collision_code=8;
		}

		// Test axis L = A1 x B0
		ra = e1[0] * abs_R.axis(0)[2] + e1[2] * abs_R.axis(0)[0];
		rb = e2[1] * abs_R.axis(2)[1] + e2[2] * abs_R.axis(1)[1];
		dist=fabs(t[0] * R.axis(0)[2] - t[2] * R.axis(0)[0]) -( ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			collision_code=9;
		}

		// Test axis L = A1 x B1
		ra = e1[0] * abs_R.axis(1)[2] + e1[2] * abs_R.axis(1)[0];
		rb = e2[0] * abs_R.axis(2)[1] + e2[2] * abs_R.axis(0)[1];
		dist=fabs(t[0] * R.axis(1)[2] - t[2] * R.axis(1)[0]) - (ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			collision_code=10;
		}

		// Test axis L = A1 x B2
		ra = e1[0] * abs_R.axis(2)[2] + e1[2] * abs_R.axis(2)[0];
		rb = e2[0] * abs_R.axis(1)[1] + e2[1] * abs_R.axis(0)[1];
		dist=fabs(t[0] * R.axis(2)[2] - t[2] * R.axis(2)[0]) - (ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			collision_code=11;
		}

		// Test axis L = A2 x B0
		ra = e1[0] * abs_R.axis(0)[1] + e1[1] * abs_R.axis(0)[0];
		rb = e2[1] * abs_R.axis(2)[2] + e2[2] * abs_R.axis(1)[2];
		dist=fabs(t[1] * R.axis(0)[0] - t[0] * R.axis(0)[1]) -(ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			collision_code=12;
		}

		// Test axis L = A2 x B1
		ra = e1[0] * abs_R.axis(1)[1] + e1[1] * abs_R.axis(1)[0];
		rb = e2[0] * abs_R.axis(2)[2] + e2[2] * abs_R.axis(0)[2];
		dist=fabs(t[1] * R.axis(1)[0] - t[0] * R.axis(1)[1]) - (ra + rb);
		if (dist>0)
			return 0;
		if (dist>penetration)
		{
			penetration=dist;
			collision_code=13;
		}

		// Test axis L = A2 x B2
		ra = e1[0] * abs_R.axis(2)[1] + e1[1] * abs_R.axis(2)[0];
		rb = e2[0] * abs_R.axis(1)[2] + e2[1] * abs_R.axis(0)[2];
		dist=fabs(t[1] * R.axis(2)[0] - t[0] * R.axis(2)[1]) - (ra + rb);
		if (dist>0)
			return 0;
		if (dist>penetration)
		{
			penetration=dist;
			collision_code=14;
		}
	}

	if (collision_code<3)
	{
		o_normal=box1_mtx.axis(collision_code);
	}
	else if (collision_code<6)
	{
		o_normal=box2_mtx.axis(collision_code-3);

	}
	else
	{
		int axis_1=(collision_code-6) / 3;
		int axis_2=(collision_code-6) % 3;

		o_normal.cross(box1_mtx.axis(axis_1),box2_mtx.axis(axis_2));
		o_normal*=dot(o_normal,box2_mtx.t-box1_mtx.t);
		o_normal.normalize();
		vec3 p1,p2;
		box1->get_extreme_point(p1,box1_mtx,o_normal);
		box2->get_extreme_point(p2,box2_mtx,-o_normal);
		vec3 v1,v2;
		line_line_closest_points(p1,box1_mtx.axis(axis_1),p2,box2_mtx.axis(axis_2),v1,v2);

	}

	o_normal*=dot(o_normal,box2_mtx.t-box1_mtx.t);

	//no most meg kellene keresni az utkozesi pontokat

	return 1;
}


#endif//_shapeintersection_h_
