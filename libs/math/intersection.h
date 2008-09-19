#ifndef _intersection_h_
#define _intersection_h_

namespace math
{
	int line_quad_intersect(vec3& o_pos, vec3 i_s1, vec3 i_s2, vec3 i_q1, vec3 i_q2, vec3 i_q3, vec3 i_q4)
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
			float u = dot(pd, m); // scalartriple(psegment, pq4, pq3);

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

	int segment_sphere_intersect(float& o_t, const vec3& i_s1, const vec3& i_s2, const vec3& i_spherecenter, float i_radius)
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



	int segment_cylinder_intersect(float& o_t, const vec3& i_s1, const vec3& i_s2, const vec3& i_c1, const vec3& i_c2, float i_radius)
	{
		vec3 d = i_c2 – i_c1, m = i_s1 – i_c1, n = i_s2 – i_s1;
		float md = dot(m, d);
		float nd = dot(n, d);
		float dd = dot(d, d);
		// Test if segment fully outside either endcap of cylinder
		if (md < 0.0f && md + nd < 0.0f) return 0; // Segment outside ’p’ side of cylinder
		if (md > dd && md + nd > dd) return 0; // Segment outside ’q’ side of cylinder
		float nn = dot(n, n);
		float mn = dot(m, n);
		float a = dd * nn – nd * nd;
		float k = dot(m, m) – i_radius * i_radius;
		float c = dd * k – md * md;
		float t;

		if (abs(a) < 0.00001f)
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

		float b = dd * mn – nd * md;
		float discr = b * b – a * c;

		if (discr < 0.0f)
			return 0;

		t = (-b – sqrtf(discr)) / a;

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
			o_t = (dd – md) / nd;

			return k + dd – 2 * md + o_t * (2 * (mn – nd) + o_t * nn) <= 0.0f;
		}

		o_t=t;
		return 1;
	}

	int segment_triangle_intersect(float& o_t,vec3 p, vec3 q, vec3 a, vec3 b, vec3 c)
	{
		float u,v,w,t;
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
#endif//_intersection_h_