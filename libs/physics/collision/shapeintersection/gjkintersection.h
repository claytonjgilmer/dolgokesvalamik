#ifndef _gjkintersection_h_
#define _gjkintersection_h_

#include "math/dvec3.h"
#include "math/mtx4x3.h"

struct convex_mesh_data_t;

struct gjk_intersection
{
	convex_mesh_data_t* obj1;
	convex_mesh_data_t* obj2;
//	mtx4x3 mtx1;
//	mtx4x3 mtx2;

	dvec3 simplex[4];
	dvec3 simplex1[4];
	dvec3 simplex2[4];
	int simplex_size;
	int iteration;

	dvec3 dir;
	dvec3 prevdir;
	bool result;
	int out;

	gjk_intersection(convex_mesh_data_t* obj1, convex_mesh_data_t* obj2,const mtx4x3& mtx1, const mtx4x3& mtx2, const dvec3& initdir=dvec3(0,0,0));
	void calculate_contact_data();

	void get_min_subsimplex();
	bool origo_in_segmentvertex_region(int simplex_index1,int simplex_index2);
	bool origo_in_trianglevertex_region(int simplex_index1,int simplex_index2,int simplex_index3);
	bool origo_in_triangleedge_region(int simplex_index1,int simplex_index2,int simplex_index3);
	bool origo_in_tetrahedronvertex_region(int simplex_index1,int simplex_index2,int simplex_index3, int simplex_index4);
	bool origo_in_tetrahedronedge_region(int simplex_index1,int simplex_index2,int simplex_index3, int simplex_index4);
	bool origo_in_tetrahedronface_region(int simplex_index1,int simplex_index2,int simplex_index3, int simplex_index4);
};

struct convex_hull_t;
struct gjk_intersection2
{
	convex_hull_t* obj1;
	convex_hull_t* obj2;
//	mtx4x3 mtx1;
//	mtx4x3 mtx2;

	dvec3 simplex[4];
	dvec3 simplex1[4];
	dvec3 simplex2[4];
	int simplex_size;
	int iteration;

	dvec3 dir;
	dvec3 prevdir;
	bool result;
	int out;

	gjk_intersection2(convex_hull_t* obj1, convex_hull_t* obj2,const mtx4x3& mtx1, const mtx4x3& mtx2, const dvec3& initdir=dvec3(0,0,0));
	void calculate_contact_data();

	void get_min_subsimplex();
	bool origo_in_segmentvertex_region(int simplex_index1,int simplex_index2);
	bool origo_in_trianglevertex_region(int simplex_index1,int simplex_index2,int simplex_index3);
	bool origo_in_triangleedge_region(int simplex_index1,int simplex_index2,int simplex_index3);
	bool origo_in_tetrahedronvertex_region(int simplex_index1,int simplex_index2,int simplex_index3, int simplex_index4);
	bool origo_in_tetrahedronedge_region(int simplex_index1,int simplex_index2,int simplex_index3, int simplex_index4);
	bool origo_in_tetrahedronface_region(int simplex_index1,int simplex_index2,int simplex_index3, int simplex_index4);
};

#endif//_gjkintersection_h_