#ifndef _boxboxintersection_h_
#define _boxboxintersection_h_

#include "math/vec2.h"

#define BOX_EPSILON .0001f
#define BOX_CUTOFF (1.0f-BOX_EPSILON)
MLINLINE void rectangle_convex_polygon_clipping(vec3 out[], int& out_count, vec3 in[], int in_count, int in_x, int in_y, vec2 rect_extent);
MLINLINE int test_box_box_intersect(shape_t* i_shape1, const mtx4x3& i_body1_mtx,
									shape_t* i_shape2, const mtx4x3& i_body2_mtx,
									vec3 o_contact_array[][2],
									vec3& o_normal,
									uint32& o_contact_num)
{
	assertion(i_shape1->type==shape_type_box && i_shape2->type==shape_type_box);
	box_shape* box[2];
	box[0]=(box_shape*)i_shape1;
	box[1]=(box_shape*)i_shape2;

	mtx4x3 box_mtx[2];
	box_mtx[0].multiply(box[0]->pos,i_body1_mtx);
	box_mtx[1].multiply(box[1]->pos,i_body2_mtx);

	mtx3x3 R; R.multiplytransposed3x3(box_mtx[1],box_mtx[0]);

	float ra, rb;
	mtx3x3 abs_R;

	// Compute translation vector t
	// Bring translation into a's coordinate frame
	vec3 t;  box_mtx[0].transformtransposed3x3(t,box_mtx[1].t - box_mtx[0].t);

	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	bool parallel_edge_pair=false;

	float penetration=-FLT_MAX;
	int axis_code;
	const vec3& e1=box[0]->extent;
	const vec3& e2=box[1]->extent;
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
			axis_code=i;
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
			axis_code=3+i;
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
			axis_code=6;
		}

		// Test axis L = A0 x B1
		ra = e1[1] * abs_R.axis(1)[2] + e1[2] * abs_R.axis(1)[1];
		rb = e2[0] * abs_R.axis(2)[0] + e2[2] * abs_R.axis(0)[0];
		dist=fabs(t[2] * R.axis(1)[1] - t[1] * R.axis(1)[2]) -( ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			axis_code=7;
		}

		// Test axis L = A0 x B2
		ra = e1[1] * abs_R.axis(2)[2] + e1[2] * abs_R.axis(2)[1];
		rb = e2[0] * abs_R.axis(1)[0] + e2[1] * abs_R.axis(0)[0];
		dist=fabs(t[2] * R.axis(2)[1] - t[1] * R.axis(2)[2]) - (ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			axis_code=8;
		}

		// Test axis L = A1 x B0
		ra = e1[0] * abs_R.axis(0)[2] + e1[2] * abs_R.axis(0)[0];
		rb = e2[1] * abs_R.axis(2)[1] + e2[2] * abs_R.axis(1)[1];
		dist=fabs(t[0] * R.axis(0)[2] - t[2] * R.axis(0)[0]) -( ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			axis_code=9;
		}

		// Test axis L = A1 x B1
		ra = e1[0] * abs_R.axis(1)[2] + e1[2] * abs_R.axis(1)[0];
		rb = e2[0] * abs_R.axis(2)[1] + e2[2] * abs_R.axis(0)[1];
		dist=fabs(t[0] * R.axis(1)[2] - t[2] * R.axis(1)[0]) - (ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			axis_code=10;
		}

		// Test axis L = A1 x B2
		ra = e1[0] * abs_R.axis(2)[2] + e1[2] * abs_R.axis(2)[0];
		rb = e2[0] * abs_R.axis(1)[1] + e2[1] * abs_R.axis(0)[1];
		dist=fabs(t[0] * R.axis(2)[2] - t[2] * R.axis(2)[0]) - (ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			axis_code=11;
		}

		// Test axis L = A2 x B0
		ra = e1[0] * abs_R.axis(0)[1] + e1[1] * abs_R.axis(0)[0];
		rb = e2[1] * abs_R.axis(2)[2] + e2[2] * abs_R.axis(1)[2];
		dist=fabs(t[1] * R.axis(0)[0] - t[0] * R.axis(0)[1]) -(ra + rb);
		if (dist>0) return 0;
		if (dist>penetration)
		{
			penetration=dist;
			axis_code=12;
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
			axis_code=13;
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
			axis_code=14;
		}
	}

	if (axis_code>5) //edge-edge
	{
		int axis_1=(axis_code-6) / 3;
		int axis_2=(axis_code-6) % 3;

		o_normal.cross(box_mtx[0].axis(axis_1),box_mtx[1].axis(axis_2));
		o_normal*=dot(o_normal,box_mtx[1].t-box_mtx[0].t);
		o_normal.normalize();
		vec3 p1,p2;
		box[0]->get_extreme_point(p1,box_mtx[0],o_normal);
		box[1]->get_extreme_point(p2,box_mtx[1],-o_normal);
		vec3 v1,v2;
		line_line_closest_points(p1,box_mtx[0].axis(axis_1),p2,box_mtx[1].axis(axis_2),v1,v2);
		o_contact_num=1;
		o_contact_array[0][0]=box_mtx[0].transformtransposed(v2);
		o_contact_array[0][1]=box_mtx[1].transformtransposed(v1);
		return 1;
	}

	int this_index, other_index;

	if (axis_code<3)
	{
		this_index=0;
		other_index=1;
	}
	else
	{
		this_index=1;
		other_index=0;
		axis_code-=3;
	}

	/*
	tehat mi a faszt is csinalunk
	a masik boxbol kivalasztjuk a megfelelo face-t
	azaz a legjobban illeszkedo tengelyt
	aztan a masik ket tengelyt
	a legjobban illeszkedo tengely alljon "jofele"
	a masik ketto mindegy
	no most johet a kvad
	*/

	o_normal=box_mtx[this_index].axis(axis_code);
	o_normal*=sign(dot(o_normal,box_mtx[1].t-box_mtx[0].t));
	int axis_1=nextnumbermodulo3(axis_code);
	int axis_2=nextnumbermodulo3(axis_1);

	const mtx4x3& m1=box_mtx[this_index];
	const mtx4x3& m2=box_mtx[other_index];

	float dot_normal[3];
	dot_normal[0]=o_normal.dot(m2.x);
	dot_normal[1]=o_normal.dot(m2.y);
	dot_normal[2]=o_normal.dot(m2.z);

	int best_dir=0;
	float best_val;

	if (abs(dot_normal[1])>abs(dot_normal[0]))
	{
		best_dir=1;
		best_val=abs(dot_normal[1]);
	}

	if (abs(dot_normal[2])>best_val)
		best_dir=2;

	int other_axis_1=nextnumbermodulo3(best_dir);
	int other_axis_2=nextnumbermodulo3(other_axis_1);

	vec3 main_axis=m2.axis(best_dir)*sign(dot(m2.axis(best_dir),m1.t-m2.t));
	vec3 quad[4];
	vec3 x=box[other_index]->extent[other_axis_1]*m2.axis(other_axis_1);
	vec3 y=box[other_index]->extent[other_axis_2]*m2.axis(other_axis_2);
	vec3 z=box[other_index]->extent[best_dir]*main_axis;
	quad[0]=m1.transformtransposed(m2.t+z-x-y);
	quad[1]=m1.transformtransposed(m2.t+z+x-y);
	quad[2]=m1.transformtransposed(m2.t+z+x+y);
	quad[3]=m1.transformtransposed(m2.t+z-x+y);

	vec3 out[8];
	int out_count;


	rectangle_convex_polygon_clipping(out,out_count,quad,4,axis_1,axis_2,vec2(box[this_index]->extent[axis_1],box[this_index]->extent[axis_2]));


	o_contact_num=0;

	float main_extent=box[this_index]->extent[axis_code];
	vec3 norm_rel=o_normal*sign(dot(o_normal,m2.t-m1.t));

	const mtx4x3* const body_mtx[2]={&i_body1_mtx,&i_body2_mtx};

	for (int n=0; n<out_count; ++n)
	{
		float pen=main_extent-abs(out[n][axis_code]);

		if (pen<-.01f)
			continue;

		vec3 abs_contact;

		m1.transform(abs_contact,out[n]);
		body_mtx[other_index]->transformtransposed(o_contact_array[o_contact_num][other_index],abs_contact);
		body_mtx[this_index]->transformtransposed(o_contact_array[o_contact_num][this_index],abs_contact+pen*norm_rel);
		++o_contact_num;
	}






//	o_normal*=dot(o_normal,box_mtx[1].t-box_mtx[0].t);
//	o_normal.normalize();

	return 1;
}

struct float_compare_less
{
	int operator()(float x, float y) const
	{
		return x<y ? 0 : 1;
	}
};

struct float_compare_greater
{
	int operator()(float x, float y) const
	{
		return x>y ? 0 : 1;
	}
};

template <typename compare_fn>
MLINLINE void poly_line_clipping(vec3 out[], int& out_count, vec3 in[], int in_count, int coord, const compare_fn& comp, float val)
{
	out_count=0;
	int prev_index=in_count-1;
	int prev=comp(in[prev_index][coord],val);

	for (int n=0; n<in_count; ++n)
	{
		int act=comp(in[n][coord],val);

		if (act ^ prev)
		{
			float t=(val-in[prev_index][coord])/(in[n][coord]-in[prev_index][coord]);
			out[out_count++]=in[prev_index]+t*(in[n]-in[prev_index]);
		}

		if (act)
			out[out_count++]=in[n];

		prev=act;
		prev_index=n;
	}
}

MLINLINE void rectangle_convex_polygon_clipping(vec3 out[], int& out_count, vec3 in[], int in_count, int in_x, int in_y, vec2 rect_extent)
{
	vec3 tmp[8];

	poly_line_clipping<float_compare_less>(tmp,out_count,in,in_count,in_x,float_compare_less(),-rect_extent.x);
	poly_line_clipping<float_compare_less>(out,out_count,tmp,out_count,in_y,float_compare_less(),-rect_extent.y);
	poly_line_clipping<float_compare_greater>(tmp,out_count,out,out_count,in_x,float_compare_greater(),rect_extent.x);
	poly_line_clipping<float_compare_greater>(out,out_count,tmp,out_count,in_y,float_compare_greater(),rect_extent.y);
}

#endif//_boxboxintersection_h_