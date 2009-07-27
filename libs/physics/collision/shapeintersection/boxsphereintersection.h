#ifndef _boxsphereintersection_h_
#define _boxsphereintersection_h_

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

#endif//_boxsphereintersection_h_