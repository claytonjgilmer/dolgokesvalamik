#ifndef _intersection_h_
#define _intersection_h_

#include "math/vec3.h"
#include "math/aabox.h"

MLINLINE int line_quad_intersect(vec3& o_pos, vec3 i_s1, vec3 i_s2, vec3 i_q1, vec3 i_q2, vec3 i_q3, vec3 i_q4)
{
	vec3 psegment = i_s2 - i_s1;
	vec3 pq1 = i_q1 - i_s1;
	vec3 pq2 = i_q2 - i_s1;
	vec3 pq3 = i_q3 - i_s1;

	// Determine which triangle to test against by testing against diagonal first
	vec3 m = cross(pq3, psegment);
	float v = dot(pq1, m); // scalartriple(psegment, pq1, pq3);

	if (v >= 0.0f)
	{
		//q1-q3-q3 haromszog
		float u = -dot(pq2, m); // scalartriple(psegment, pq3, pq2);

		if (u < 0.0f)
			return 0;

		float w = scalartriple(psegment, pq2, pq1);

		if (w < 0.0f)
			return 0;
		float denom = 1.0f / (u + v + w);
		u *= denom;
		v *= denom;
		w *= denom;
		o_pos = u*i_q1 + v*i_q2 + w*i_q3;
	}
	else
	{
		//q4-q1-q3
		vec3 pq4 = i_q4 - i_s1;
		float u = dot(pq4, m); // scalartriple(psegment, pq4, pq3);

		if (u < 0.0f)
			return 0;

		float w = scalartriple(psegment, pq1, pq4);

		if (w < 0.0f)
			return 0;

		v = -v;
		// Compute r, r = u*a + v*d + w*c, from barycentric coordinates (u, v, w)
		float denom = 1.0f / (u + v + w);
		u *= denom;
		v *= denom;
		w *= denom; // w = 1.0f - u - v;
		o_pos = u*i_q1 + v*i_q4 + w*i_q3;
	}
	return 1;
}

MLINLINE int segment_sphere_intersect(float& o_t, const vec3& i_s1, const vec3& i_s2, const vec3& i_spherecenter, float i_radius)
{
	vec3 m = i_s1 - i_spherecenter;
	vec3 d=i_s2-i_s1; float l=d.length(); d/=l;
	float b = dot(m, d);
	float c = dot(m, m) - i_radius*i_radius;

	if (c > 0.0f && b > 0.0f)
		return 0;

	float discr = b*b - c;

	if (discr < 0.0f)
		return 0;

	float t;

	t = -b - sqrtf(discr);

	if (t>l)
		return 0;

	if (t < 0.0f)
		t = 0.0f;

	o_t=t;
	return 1;
}



MLINLINE int segment_cylinder_intersect(float& o_t, const vec3& i_s1, const vec3& i_s2, const vec3& i_c1, const vec3& i_c2, float i_radius)
{
	vec3 d = i_c2-i_c1, m = i_s1-i_c1, n = i_s2-i_s1;
	float md = dot(m, d);
	float nd = dot(n, d);
	float dd = dot(d, d);

	if (md < 0.0f && md + nd < 0.0f)
		return 0;

	if (md > dd && md + nd > dd)
		return 0;

	float nn = dot(n, n);
	float mn = dot(m, n);
	float a = dd * nn-nd * nd;
	float k = dot(m, m) - i_radius * i_radius;
	float c = dd * k - md * md;
	float t;

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

	float b = dd * mn - nd * md;
	float discr = b * b - a * c;

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

MLINLINE int segment_capsule_intersect(float& o_t, const vec3& i_s1, const vec3& i_s2, const vec3& i_c1, const vec3& i_c2, float i_radius)
{
	vec3 d = i_c2-i_c1, m = i_s1-i_c1, n = i_s2-i_s1;
	float md = dot(m, d);
	float nd = dot(n, d);
	float dd = dot(d, d);

	if (md < 0.0f && md + nd < 0.0f)
		return 0;

	if (md > dd && md + nd > dd)
		return 0;

	float nn = dot(n, n);
	float mn = dot(m, n);
	float a = dd * nn-nd * nd;
	float k = dot(m, m) - i_radius * i_radius;
	float c = dd * k - md * md;
	float t;

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

	float b = dd * mn - nd * md;
	float discr = b * b - a * c;

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

MLINLINE int segment_triangle_intersect(float& o_t,vec3 p, vec3 q, vec3 a, vec3 b, vec3 c)
{
	float v,w,t;
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 qp = p - q;
	vec3 n = cross(ab, ac);
	float d = dot(qp, n);

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
	float dot11=dot(cross1,start2);
	float dot12=dot(cross1,end2);

	if (dot11*dot12>=0)
		return 0;

	vec3 cross2; cross2.cross(start2,end2);
	float dot21=dot(cross2,start1);
	float dot22=dot(cross2,end1);

	if (dot21*dot22>=0)
		return 0;

	vec3 v1=start1+dot21/(dot21-dot22)*(end1-start1);
	vec3 v2=start2+dot11/(dot11-dot12)*(end2-start2);

	if (dot(v1,v2)<0)
		return false;

	v=v1;
	v.normalize();
	return 1;
}

MLINLINE uint32 obb_obb_intersect(vec3& v, const vec3& center1, const vec3& extent1, const mtx4x3& mtx1,
								  const vec3& center2, const vec3& extent2, const mtx4x3& mtx2)
{
	mtx4x3 trf_1_to_2; trf_1_to_2.multiplytransposed(mtx1,mtx2);
	vec3 centerdir=mtx1.transform(center2)-mtx2.transform(center1);
	vec3 centerdir1=mtx1.transformtransposed3x3(centerdir);
	vec3 centerdir2=mtx2.transformtransposed3x3(centerdir);

	float pen=-FLT_MAX;
	vec3 normal;

	for (int n=0; n<3; ++n)
	{
		float size1=extent1[n];
		float size2=extent2[0]*fabsf(trf_1_to_2.x[n])+extent2[1]*fabsf(trf_1_to_2.y[n])+extent2[2]*fabsf(trf_1_to_2.z[n]);

		float dist=fabsf(centerdir1[n])-(size1+size2);
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
		float size1=extent2[n];
		float size2=extent1[0]*fabsf(trf_1_to_2.axis(n).x)+extent1[1]*fabsf(trf_1_to_2.axis(n).y)+extent1[2]*fabsf(trf_1_to_2.axis(n).z);

		float dist=fabsf(centerdir2[n])-(size1+size2);
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

			float axislength=actaxis.squarelength();

			if (axislength<0.00001f)
				continue;

			float projectedsizebox1=extent1.x*fabsf(dot(actaxis,mtx1.x))+
				extent1.y*fabsf(dot(actaxis,mtx1.y))+
				extent1.z*fabsf(dot(actaxis,mtx1.z));

			float projectedsizebox2=extent2.x*fabsf(dot(actaxis,mtx2.x))+
				extent2.y*fabsf(dot(actaxis,mtx2.y))+
				extent2.z*fabsf(dot(actaxis,mtx2.z));

			float projecteddist=dot(actaxis,centerdir);
			float separation=fabsf(projecteddist)-(projectedsizebox1+projectedsizebox2);

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

MLINLINE uint32 sphere_sphere_intersect(vec3& v,const vec3& center1, float radius1, const vec3& center2, float radius2)
{
	vec3 dir=center2-center1;
	float sql=dir.squarelength();
	if (sql>sqr(radius1+radius2))
		return 0;
	v=dir; v.normalize(); v*=radius1+radius2-sqrtf(sql);
	return 1;

}
#endif//_intersection_h_
