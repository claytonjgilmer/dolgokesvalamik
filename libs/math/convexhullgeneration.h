#ifndef _convexhullgeneration_h_
#define _convexhullgeneration_h_

#include "convexhull.h"

struct convex_hull_desc
{
	vector<vec3> vertex_array;
	float face_thickness;

	convex_hull_desc()
	{
		face_thickness=0.0001f;
	}
};
convex_hull generate_convex_hull(const convex_hull_desc& hull_desc);
#endif//_convexhullgeneration_h_
