#ifndef _mprintersection_h_
#define _mprintersection_h_

#include "math/dvec3.h"
#include "math/mtx4x3.h"

template<typename T1, typename T2>
struct mpr_intersection
{
	mpr_intersection(T1* obj1, T2* obj2,const mtx4x3& mtx1, const mtx4x3& mtx2);

	int iteration;
	bool result;

	vec3 point1;
	vec3 point2;
	vec3 normal;

	bool test_intersection(T1* p1, const mtx4x3& m1, T2* p2, const mtx4x3& m2, vec3& returnNormal, vec3& point1, vec3& point2);

};











template <typename T>
MLINLINE void get_extremal_vertex(T* obj, const dvec3& dir, const mtx4x3& mtx, dvec3& support)
{
	vec3 localdir; dvec3_to_vec3(localdir,dir);
	localdir=mtx.transformtransposed3x3(localdir);
	vec3 localsupport;
	obj->get_extreme_point(localsupport,localdir);
	//	get_extremal_vertex2(obj,localdir,localsupport);
	support=dvec3(mtx.transform(localsupport));
}


template<typename T1,typename T2>
MLINLINE bool mpr_intersection<T1,T2>::test_intersection(T1* p1, const mtx4x3& m1, T2* p2, const mtx4x3& m2, vec3& returnNormal, vec3& point1, vec3& point2)
{
	int32 supportCount = 0;

	static f32 kCollideEpsilon = 1e-3f;
	static int32 callCount = 0;

	callCount++;

	// v0 = center of Minkowski sum
	dvec3 v01(m1.transform(p1->get_center()));
	dvec3 v02(m2.transform(p2->get_center()));

	dvec3 v0 = v02 - v01;

	// v1 = support in direction of origin
	dvec3 n = -v0;
	supportCount++;

	dvec3 v11,v12;
	n.normalize();
	get_extremal_vertex(p1,-n,m1,v11);
	get_extremal_vertex(p2,n,m2,v12);
	dvec3 v1 = v12 - v11;

	if (dot(v1,n) <= 0)
		return false;

	// v2 - support perpendicular to v1,v0
	n.cross(v1,v0);
	if (n.squarelength()<0.0000001)
	{
		n = v0;
		n.normalize();
		dvec3_to_vec3(returnNormal,n);
		dvec3_to_vec3(point1,v11);
		dvec3_to_vec3(point2,v12);
		return true;
	}

	supportCount++;
	dvec3 v21,v22;
	n.normalize();
	get_extremal_vertex(p1,-n,m1,v21);
	get_extremal_vertex(p2,n,m2,v22);
	dvec3 v2 = v22 - v21;

	if (dot(v2,n) <= 0)
		return false;

	// Determine whether origin is on + or - side of plane (v1,v0,v2)
	n.cross((v1 - v0),(v2 - v0));
	f64 dist = dot(n,v0);

	assertion(n.squarelength()>.0000001);

	// If the origin is on the - side of the plane, reverse the direction of the plane
	if (dist > 0)
	{
		swap(v1, v2);
		swap(v11, v21);
		swap(v12, v22);
		n = -n;
	}
	///
	// Phase One: Identify a portal

	while (1)
	{
		// Obtain the support point in a direction perpendicular to the existing plane
		// Note: This point is guaranteed to lie off the plane
		supportCount++;
		dvec3 v31,v32;
		n.normalize();
		get_extremal_vertex(p1,-n,m1,v31);
		get_extremal_vertex(p2,n,m2,v32);

		dvec3 v3 = v32 - v31;

		if (dot(v3,n) <= 0)
			return false;

		// If origin is outside (v1,v0,v3), then eliminate v2 and loop
		if (dot(cross(v1,v3),v0) < 0)
		{
			v2 = v3;
			v21 = v31;
			v22 = v32;
			n.cross((v1 - v0),(v3 - v0));
			continue;
		}

		// If origin is outside (v3,v0,v2), then eliminate v1 and loop
		if (dot(cross(v3,v2),v0) < 0)
		{
			v1 = v3;
			v11 = v31;
			v12 = v32;
			n.cross((v3 - v0),(v2 - v0));
			continue;
		}

		bool hit = false;

		///
		// Phase Two: Refine the portal

		int32 phase2 = 0;

		// We are now inside of a wedge...
		while (1)
		{
			phase2++;
			// Compute normal of the wedge face
			n.cross((v1 - v2),(v3 - v1));
			// Can this happen???  Can it be handled more cleanly?
			if (n.squarelength()<0.0000001)
			{
				assertion(0);
				return false;
			}

			n.normalize();

			f64 d = dot(n, v1);

			if (d <= 0)
			{
				// HIT!!!
				hit = true;
			}

			// Find the support point in the direction of the wedge face
			supportCount++;
			dvec3 v41,v42;
			get_extremal_vertex(p1,n,m1,v41);
			get_extremal_vertex(p2,-n,m2,v42);
			dvec3 v4 = v42 - v41;

			f64 delta = dot((v3 - v4), n);
			f64 separation = dot(v4, n);

			// If the boundary is thin enough or the origin is outside the support plane for the newly discovered vertex, then we can terminate
			if ( delta <= kCollideEpsilon || separation >= 0)
			{
				if (hit)
				{
					dvec3_to_vec3(returnNormal,n);

					vec3 d1(dvec3_to_vec3(v0-v1));
					vec3 d2(dvec3_to_vec3(v0-v2));
					vec3 d3(dvec3_to_vec3(v0-v3));
					vec3 d4(dvec3_to_vec3(v0));

					vec3 t=solve_3x3(mtx3x3(d1,d2,d3),d4);

					dvec3 common_point=-(t[0]*(v01-v11)+t[1]*(v01-v21)+t[2]*(v01-v31)-v01);

					point1=dvec3_to_vec3(common_point+dot(n,v41-common_point)*n);
					point2=dvec3_to_vec3(common_point+dot(n,v42-common_point)*n);

					return true;
					dvec3 pp2=t[0]*(v02-v12)+t[1]*(v02-v22)+t[2]*(v02-v32)-v02;
					//p1-nek es p2-nek egyenlonek kell lennie!





					dvec3 V=v2-v1, W=v3-v1, P=-v1;
					double mul=(dot(V,V)*dot(W,W)-(sqr(dot(V,W))));

					if (mul>-0.000001 && mul<0.000001) //ha deformalt a haromszog
					{
						dvec3 dir=v1-v2;
						double time=dot(v1,dir)/dir.squarelength();
						dvec3_to_vec3(point1,v11+time*(v21-v11));
						dvec3_to_vec3(point2,v12+time*(v22-v12));
					}
					else
					{

						double a2=(dot(V,V)*dot(W,P)-dot(V,W)*dot(V,P))/mul;
						double a1=(dot(V,P)-a2*dot(V,W))/dot(V,V);

#ifdef _DEBUG
						if (!((a2>-0.01 && a2<1.01)) || !((a1>-0.01 && a1<1.01)))
						{
							float x1,y1,z1;
							float x2,y2,z2;

							m1.get_euler(x1,y1,z1);
							m2.get_euler(x2,y2,z2);

							x1=radtodegree(x1);
							y1=radtodegree(y1);
							z1=radtodegree(z1);
							x2=radtodegree(x2);
							y2=radtodegree(y2);
							z2=radtodegree(z2);

							PRINT("matrix1: t:%f %f %f r:%f %f %f\n",m1.t.x,m1.t.y,m1.t.z,x1,y1,z1);
							PRINT("matrix2: t:%f %f %f r:%f %f %f\n",m2.t.x,m2.t.y,m2.t.z,x2,y2,z2);
						}
#endif
						assertion(a1>-0.01 && a1<1.01);
						assertion(a2>-0.01 && a2<1.01);
//						assertion(a1>-10 && a1<10);

						dvec3_to_vec3(point1,v11+a1*(v21-v11)+a2*(v31-v11));
						dvec3_to_vec3(point2,v12+a1*(v22-v12)+a2*(v32-v12));
					}
				}

				return hit;
			}

			// Compute the tetrahedron dividing face (v4,v0,v1)
			f64 d1 =dot(cross(v4,v1),v0);

			// Compute the tetrahedron dividing face (v4,v0,v2)
			f64 d2 = dot(cross(v4,v2),v0);

			// Compute the tetrahedron dividing face (v4,v0,v3)
			f64 d3 = dot(cross(v4,v3),v0);

			if (d1 < 0)
			{
				if (d2 < 0)
				{
					// Inside d1 & inside d2 ==> eliminate v1
					v1 = v4;
					v11 = v41;
					v12 = v42;
				}
				else
				{
					// Inside d1 & outside d2 ==> eliminate v3
					v3 = v4;
					v31 = v41;
					v32 = v42;
				}
			}
			else
			{
				if (d3 < 0)
				{
					// Outside d1 & inside d3 ==> eliminate v2
					v2 = v4;
					v21 = v41;
					v22 = v42;
				}
				else
				{
					// Outside d1 & outside d3 ==> eliminate v1
					v1 = v4;
					v11 = v41;
					v12 = v42;
				}
			}
		}
	}
}

template<typename T1,typename T2>
MLINLINE mpr_intersection<T1,T2>::mpr_intersection(T1* obj1, T2* obj2,const mtx4x3& mtx1, const mtx4x3& mtx2)
{
	result=test_intersection(obj1,mtx1,obj2,mtx2,normal,point1,point2);
}
#endif//_mprintersection_h_