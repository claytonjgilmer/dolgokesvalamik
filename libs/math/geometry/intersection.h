#ifndef _intersection_h_
#define _intersection_h_

#include "math/vec3.h"
#include "math/aabox.h"
#include "math/plane.h"

MLINLINE int line_quad_intersect(vec3& o_pos, vec3 i_s1, vec3 i_s2, vec3 i_q1, vec3 i_q2, vec3 i_q3, vec3 i_q4)
{
	vec3 psegment = i_s2 - i_s1;
	vec3 pq1 = i_q1 - i_s1;
	vec3 pq2 = i_q2 - i_s1;
	vec3 pq3 = i_q3 - i_s1;

	// Determine which triangle to test against by testing against diagonal first
	vec3 m = cross(pq3, psegment);
	f32 v = dot(pq1, m); // scalartriple(psegment, pq1, pq3);

	if (v >= 0.0f)
	{
		//q1-q3-q3 haromszog
		f32 u = -dot(pq2, m); // scalartriple(psegment, pq3, pq2);

		if (u < 0.0f)
			return 0;

		f32 w = scalartriple(psegment, pq2, pq1);

		if (w < 0.0f)
			return 0;
		f32 denom = 1.0f / (u + v + w);
		u *= denom;
		v *= denom;
		w *= denom;
		o_pos = u*i_q1 + v*i_q2 + w*i_q3;
	}
	else
	{
		//q4-q1-q3
		vec3 pq4 = i_q4 - i_s1;
		f32 u = dot(pq4, m); // scalartriple(psegment, pq4, pq3);

		if (u < 0.0f)
			return 0;

		f32 w = scalartriple(psegment, pq1, pq4);

		if (w < 0.0f)
			return 0;

		v = -v;
		// Compute r, r = u*a + v*d + w*c, from barycentric coordinates (u, v, w)
		f32 denom = 1.0f / (u + v + w);
		u *= denom;
		v *= denom;
		w *= denom; // w = 1.0f - u - v;
		o_pos = u*i_q1 + v*i_q4 + w*i_q3;
	}
	return 1;
}

MLINLINE int segment_sphere_intersect(f32& o_t, const vec3& i_s1, const vec3& i_s2, const vec3& i_spherecenter, f32 i_radius)
{
	vec3 m = i_s1 - i_spherecenter;
	vec3 d=i_s2-i_s1; f32 l=d.length(); d/=l;
	f32 b = dot(m, d);
	f32 c = dot(m, m) - i_radius*i_radius;

	if (c > 0.0f && b > 0.0f)
		return 0;

	f32 discr = b*b - c;

	if (discr < 0.0f)
		return 0;

	f32 t;

	t = -b - sqrtf(discr);

	if (t>l)
		return 0;

	if (t < 0.0f)
		t = 0.0f;

	o_t=t;
	return 1;
}



MLINLINE int segment_cylinder_intersect(f32& o_t, const vec3& i_s1, const vec3& i_s2, const vec3& i_c1, const vec3& i_c2, f32 i_radius)
{
	vec3 d = i_c2-i_c1, m = i_s1-i_c1, n = i_s2-i_s1;
	f32 md = dot(m, d);
	f32 nd = dot(n, d);
	f32 dd = dot(d, d);

	if (md < 0.0f && md + nd < 0.0f)
		return 0;

	if (md > dd && md + nd > dd)
		return 0;

	f32 nn = dot(n, n);
	f32 mn = dot(m, n);
	f32 a = dd * nn-nd * nd;
	f32 k = dot(m, m) - i_radius * i_radius;
	f32 c = dd * k - md * md;
	f32 t;

	if (fabsf(a) < 0.00001f)
	{
		if (c > 0.0f)
			return 0;

		if (md < 0.0f)
			t = -mn / nn;
		else if (md > dd)
			t = (nd - mn) / nn;
		else
			t = 0.0f;

		o_t=t;
		return 1;
	}

	f32 b = dd * mn - nd * md;
	f32 discr = b * b - a * c;

	if (discr < 0.0f)
		return 0;

	t = (-b - sqrtf(discr)) / a;

	if (t < 0.0f || t > 1.0f)
		return 0;

	if (md + t * nd < 0.0f)
	{
		if (nd <= 0.0f)
			return 0;

		o_t = -md / nd;
		return k + 2 * o_t * (mn + o_t  * nn) <= 0.0f;
	}
	else if (md + t * nd > dd)
	{
		if (nd >= 0.0f)
			return 0;
		o_t = (dd - md) / nd;

		return k + dd - 2 * md + o_t * (2 * (mn - nd) + o_t * nn) <= 0.0f;
	}

	o_t=t;
	return 1;
}

MLINLINE int segment_capsule_intersect(f32& o_t, const vec3& i_s1, const vec3& i_s2, const vec3& i_c1, const vec3& i_c2, f32 i_radius)
{
	vec3 d = i_c2-i_c1, m = i_s1-i_c1, n = i_s2-i_s1;
	f32 md = dot(m, d);
	f32 nd = dot(n, d);
	f32 dd = dot(d, d);

	if (md < 0.0f && md + nd < 0.0f)
		return 0;

	if (md > dd && md + nd > dd)
		return 0;

	f32 nn = dot(n, n);
	f32 mn = dot(m, n);
	f32 a = dd * nn-nd * nd;
	f32 k = dot(m, m) - i_radius * i_radius;
	f32 c = dd * k - md * md;
	f32 t;

	if (fabsf(a) < 0.00001f)
	{
		if (c > 0.0f)
			return 0;

		if (md < 0.0f)
			t = -mn / nn;
		else if (md > dd)
			t = (nd - mn) / nn;
		else
			t = 0.0f;

		o_t=t;
		return 1;
	}

	f32 b = dd * mn - nd * md;
	f32 discr = b * b - a * c;

	if (discr < 0.0f)
		return 0;

	t = (-b - sqrtf(discr)) / a;

	if (t < 0.0f || t > 1.0f)
		return 0;

	if (md + t * nd < 0.0f)
	{
		if (nd <= 0.0f)
			return 0;

		o_t = -md / nd;
		return k + 2 * o_t * (mn + o_t  * nn) <= 0.0f;
	}
	else if (md + t * nd > dd)
	{
		if (nd >= 0.0f)
			return 0;
		o_t = (dd - md) / nd;

		return k + dd - 2 * md + o_t * (2 * (mn - nd) + o_t * nn) <= 0.0f;
	}

	o_t=t;
	return 1;
}

MLINLINE int segment_triangle_intersect(f32& o_t,vec3 p, vec3 q, vec3 a, vec3 b, vec3 c)
{
	f32 v,w,t;
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 qp = p - q;
	vec3 n = cross(ab, ac);
	f32 d = dot(qp, n);

	if (d <= 0.0f)
		return 0;

	vec3 ap = p - a;
	t = dot(ap, n);

	if (t < 0.0f)
		return 0;

	if (t > d)
		return 0;

	vec3 e = cross(qp, ap);
	v = dot(ac, e);

	if (v < 0.0f || v > d)
		return 0;

	w = -dot(ab, e);

	if (w < 0.0f || v + w > d)
		return 0;

	o_t=t/d;
	return 1;
}

MLINLINE uint32 aabb_aabb_intersect(aabb_t i_box1, aabb_t i_box2)
{
	for (int n=0; n<3; ++n)
	{
		if (i_box1.min[n]>i_box2.max[n])
			return 0;
		if (i_box2.min[n]>i_box1.max[n])
			return 0;
	}

	return 1;
}

MLINLINE uint32 spherical_segment_intersect(vec3& v, const vec3& start1, const vec3& end1, const vec3& start2, const vec3& end2)
{
	vec3 cross1; cross1.cross(start1,end1);
	f32 dot11=dot(cross1,start2);
	f32 dot12=dot(cross1,end2);

	if (dot11*dot12>=0)
		return 0;

	vec3 cross2; cross2.cross(start2,end2);
	f32 dot21=dot(cross2,start1);
	f32 dot22=dot(cross2,end1);

	if (dot21*dot22>=0)
		return 0;

	if (dot11*dot22<=0 || dot21*dot12<=0)
		return 0;
#if 0
	vec3 v1=start1+dot21/(dot21-dot22)*(end1-start1);
	vec3 v2=start2+dot11/(dot11-dot12)*(end2-start2);

	if (dot(v1,v2)<0)
	{
		assertion(dot11*dot22<=0);
		return 0;
	}
#endif
	v=start1+dot21/(dot21-dot22)*(end1-start1);
	v.normalize();
#ifdef _DEBUG
	vec3 v2=start2+dot11/(dot11-dot12)*(end2-start2);
	v2.normalize();
	f32 d=dot(v,v2);
	assertion(d>0.9f);
#endif
	return 1;
}

MLINLINE uint32 obb_obb_intersect(vec3& v, const vec3& center1, const vec3& extent1, const mtx4x3& mtx1,
								  const vec3& center2, const vec3& extent2, const mtx4x3& mtx2)
{
	mtx4x3 trf_1_to_2; trf_1_to_2.multiplytransposed(mtx1,mtx2);
	vec3 centerdir=mtx1.transform(center2)-mtx2.transform(center1);
	vec3 centerdir1=mtx1.transformtransposed3x3(centerdir);
	vec3 centerdir2=mtx2.transformtransposed3x3(centerdir);

	f32 pen=-FLT_MAX;
	vec3 normal;

	for (int n=0; n<3; ++n)
	{
		f32 size1=extent1[n];
		f32 size2=extent2[0]*fabsf(trf_1_to_2.x[n])+extent2[1]*fabsf(trf_1_to_2.y[n])+extent2[2]*fabsf(trf_1_to_2.z[n]);

		f32 dist=fabsf(centerdir1[n])-(size1+size2);
		if (dist>0)
			return 0;
		if (dist>pen)
		{
			pen=dist;
			normal=mtx1.axis(n)*sign(centerdir1[n]);
		}
	}
	for (int n=0; n<3; ++n)
	{
		f32 size1=extent2[n];
		f32 size2=extent1[0]*fabsf(trf_1_to_2.axis(n).x)+extent1[1]*fabsf(trf_1_to_2.axis(n).y)+extent1[2]*fabsf(trf_1_to_2.axis(n).z);

		f32 dist=fabsf(centerdir2[n])-(size1+size2);
		if (dist>0)
			return 0;
		if (dist>pen)
		{
			pen=dist;
			normal=mtx2.axis(n)*sign(centerdir2[n]);
		}
	}

	for (int n=0; n<3; ++n)
	{
		const vec3& box1axis=mtx1.axis(n);

		for (int m=0; m<3; ++m)
		{
			const vec3& box2axis=mtx2.axis(m);

			vec3 actaxis; actaxis.cross(box1axis,box2axis);

			f32 axislength=actaxis.squarelength();

			if (axislength<0.00001f)
				continue;

			f32 projectedsizebox1=extent1.x*fabsf(dot(actaxis,mtx1.x))+
				extent1.y*fabsf(dot(actaxis,mtx1.y))+
				extent1.z*fabsf(dot(actaxis,mtx1.z));

			f32 projectedsizebox2=extent2.x*fabsf(dot(actaxis,mtx2.x))+
				extent2.y*fabsf(dot(actaxis,mtx2.y))+
				extent2.z*fabsf(dot(actaxis,mtx2.z));

			f32 projecteddist=dot(actaxis,centerdir);
			f32 separation=fabsf(projecteddist)-(projectedsizebox1+projectedsizebox2);

			if (separation>0)
				return 0;

			axislength=sqrtf(axislength);

			separation/=axislength;

			if (separation>pen)
			{
				pen=separation;
				normal=actaxis;
				normal/=axislength*sign(projecteddist);
			}
		}
	}

	v=normal*(-pen);
	return 1;
}

MLINLINE uint32 sphere_sphere_intersect(vec3& v,const vec3& center1, f32 radius1, const vec3& center2, f32 radius2)
{
	vec3 dir=center2-center1;
	f32 sql=dir.squarelength();
	if (sql>sqr(radius1+radius2))
		return 0;
	v=dir; v.normalize(); v*=radius1+radius2-sqrtf(sql);
	return 1;

}

MLINLINE void line_line_closest_points(const vec3& p1,const vec3& d1, const vec3& p2, const vec3& d2, vec3& v1, vec3& v2)
{
	vec3 p=p2-p1;

	float uaub=dot(d1,d2);
	float q1=dot(d1,p);
	float q2=dot(d2,p);

	float d=1-uaub*uaub;
//	float t1,t2;

	if (d<0.0001f)
	{
		v1=p1+q1*d1;
		v2=p2;
	}
	else
	{
		d=1.0f/d;

		v1=p1+(q1-uaub*q2)*d*d1;
		v2=p2+(uaub*q1-q2)*d*d2;
	}
}

MLINLINE vec3 segment_plane_intersect(const plane_t& p, const vec3& v1, const vec3& v2)
{
	float d1=p.get_distance(v1);
	float d2=p.get_distance(v2);

	return v1+(d1/(d1-d2))*(v2-v1);
}

MLINLINE int split_polygon(const plane_t& p, float thickness, const vec3* f, int vcount, vec3* front, int& vcount_front, vec3* back, int& vcount_back)
{
	int side_a,side_b;
	vcount_back=vcount_front=0;

	vec3 prevv=f[vcount-1];
	side_a=p.classify_point(prevv,thickness);

	int on_count=0;

	for (int n=0; n<vcount; ++n)
	{
		side_b=p.classify_point(f[n],thickness);

		if (side_b==1) //aktualis elotte
		{
			if (side_a==-1)//elozo mogotte
			{
				vec3 i=segment_plane_intersect(p,prevv,f[n]);
				front[vcount_front++]=i;
				back[vcount_back++]=i;
			}

			front[vcount_front++]=f[n];
		}
		else if (side_b==-1)//aktualis mogotte
		{
			if (side_a==1)//elozo elotte
			{
				vec3 i=segment_plane_intersect(p,prevv,f[n]);
				front[vcount_front++]=i;
				back[vcount_back++]=i;
			}
			back[vcount_back++]=f[n];
		}
		else //aktualis rajta
		{
			++on_count;
			front[vcount_front++]=f[n];
			back[vcount_back++]=f[n];
		}
		prevv=f[n];
		side_a=side_b;
	}

	return on_count==vcount;
}
#endif//_intersection_h_
