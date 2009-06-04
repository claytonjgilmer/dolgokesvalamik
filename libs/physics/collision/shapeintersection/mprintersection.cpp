#include "mprintersection.h"
#include "physics/collision/shapes/convexmeshdata.h"

inline void get_extremal_vertex(convex_mesh_data_t* obj, const dvec3& dir, const mtx4x3& mtx, dvec3& support)
{
	vec3 localdir; dvec3_to_vec3(localdir,dir);
	localdir=mtx.transformtransposed3x3(localdir);
	vec3 localsupport;
	get_extremal_vertex(obj,localdir,localsupport);
	//	get_extremal_vertex2(obj,localdir,localsupport);
	support=dvec3(mtx.transform(localsupport));
}


bool CollideAndFindPoint(convex_mesh_data_t* p1, const mtx4x3& m1, convex_mesh_data_t* p2, const mtx4x3& m2, vec3& returnNormal, vec3& point1, vec3& point2)
{
	int32 supportCount = 0;

	static f32 kCollideEpsilon = 1e-3f;
	static int32 callCount = 0;

	callCount++;

	// v0 = center of Minkowski sum
	dvec3 v01(m1.transform(p1->center));
	dvec3 v02(m2.transform(p1->center));

	dvec3 v0 = v02 - v01;

	// v1 = support in direction of origin
	dvec3 n = -v0;
	supportCount++;

	dvec3 v11,v12;
	get_extremal_vertex(p1,-n,m1,v11);
	get_extremal_vertex(p2,n,m2,v12);
	dvec3 v1 = v12 - v11;

	if (dot(v1,n) <= 0)
		return false;

	// v2 - support perpendicular to v1,v0
	n.cross(v1,v0);
	if (n.squarelength()<0.0000001)
	{
		n = v1 - v0;
		n.normalize();
		dvec3_to_vec3(returnNormal,n);
		dvec3_to_vec3(point1,v11);
		dvec3_to_vec3(point1,v12);
		return true;
	}

	supportCount++;
	dvec3 v21,v22;
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
			if (phase2 > 1)
			{
				static bool doneIt = false;
				if (!doneIt)
				{
					doneIt = true;
					CollideAndFindPoint(p1, m1, p2, m2, returnNormal, point1, point2);
					return false;
				}
			}

			// Compute normal of the wedge face
			n.cross((v2 - v1),(v3 - v1));
			// Can this happen???  Can it be handled more cleanly?
			if (n.squarelength()<0.0000001)
			{
				assertion(0);
				return true;
			}

			n.normalize();

			// Compute distance from origin to wedge face
			f64 d = dot(n, v1);

			// If the origin is inside the wedge, we have a hit
			if (d >= 0 && !hit)
			{

				dvec3_to_vec3(returnNormal,n);

				// Compute the barycentric coordinates of the origin

				f64 b0 = dot(cross(v1 , v2), v3);
				f64 b1 = dot(cross(v3 , v2), v0);
				f64 b2 = dot(cross(v0 , v1), v3);
				f64 b3 = dot(cross(v2 , v1), v0);

				f64 sum = b0 + b1 + b2 + b3;

				if (sum <= 0)
				{
					b0 = 0;
					b1 = dot(cross(v2,v3),n);
					b2 = dot(cross(v3,v1),n);
					b3 = dot(cross(v1,v2),n);

					sum = b1 + b2 + b3;
				}

				f64 inv = 1.0f / sum;

				dvec3 p1 = (b0 * v01 + b1 * v11 + b2 * v21 + b3 * v31) * inv;
				dvec3_to_vec3(point1,p1);

				dvec3 p2 = (b0 * v02 + b1 * v12 + b2 * v22 + b3 * v32) * inv;
				dvec3_to_vec3(point2, p2);

				// HIT!!!
				hit = true;
			}

			// Find the support point in the direction of the wedge face
			supportCount++;
			dvec3 v41,v42;
			get_extremal_vertex(p1,-n,m1,v41);
			get_extremal_vertex(p2,n,m2,v42);
			dvec3 v4 = v42 - v41;

			f64 delta = dot((v4 - v3), n);
			f64 separation = -dot(v4, n);

			// If the boundary is thin enough or the origin is outside the support plane for the newly discovered vertex, then we can terminate
			if ( delta <= kCollideEpsilon || separation >= 0 /*|| phase2 > 300*/ )
			{
				if (hit)
					dvec3_to_vec3(returnNormal,n);

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


mpr_intersection::mpr_intersection(convex_mesh_data_t* obj1, convex_mesh_data_t* obj2,const mtx4x3& mtx1, const mtx4x3& mtx2)
{
	result=CollideAndFindPoint(obj1,mtx1,obj2,mtx2,normal,point1,point2);
}