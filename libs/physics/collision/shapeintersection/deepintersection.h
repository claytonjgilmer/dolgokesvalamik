#ifndef _deepintersection_h_
#define _deepintersection_h_

#include "math/mtx4x3.h"
#include "math/dvec3.h"
#include "math/geometry/convexhull.h"
struct convex_hull_t;
struct deep_intersection
{
	convex_hull_t* obj1;
	convex_hull_t* obj2;
	mtx4x3 mtx1;
	mtx4x3 mtx2;
	mtx4x3 trf_1_to_2;

	enum
	{
		edge_edge,
		face_vertex,
		vertex_face
	} ;

	int state;

	bool result;
	float depth;

	deep_intersection(convex_hull_t* i_obj1, convex_hull_t* i_obj2, const mtx4x3& i_mtx1, const mtx4x3& i_mtx2, const vec3& initdir=to_vec3(0,0,0),edge_data* i_v1=NULL,edge_data* i_v2=NULL, int state=0);
	bool report_min_pair(edge_data* i_v1, edge_data* i_v2);
	bool report_min_pair_face(edge_data* i_v1, edge_data* i_v2);
	bool report_min_pair_edge(edge_data* i_v1, edge_data* i_v2);

	edge_data* v1,*v2;
	vec3 normal;
	int itnum;

};
#endif//_deepintersection_h_
