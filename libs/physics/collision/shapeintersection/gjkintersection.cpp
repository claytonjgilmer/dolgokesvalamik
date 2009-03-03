#include "gjkintersection.h"
#include "physics/collision/shapes/convexmeshdata.h"
#include "math/geometry/convexhull.h"

#define  MAX_ITERATION_COUNT 20


//////////////////////////////////////////////////////////////////////////
//		EBBE NE IRJ!!!!!!!!!!!!!!!!!!!!
//////////////////////////////////////////////////////////////////////////



void get_extremal_vertex(convex_mesh_data_t* obj, const dvec3& dir, const mtx4x3& mtx, dvec3& support)
{
	vec3 localdir; localdir.set((f32)dir.x,(f32)dir.y,(f32)dir.z);
	localdir=mtx.transformtransposed3x3(localdir);
	vec3 localsupport;
	get_extremal_vertex(obj,localdir,localsupport);
//	get_extremal_vertex2(obj,localdir,localsupport);
	support=dvec3(mtx.transform(localsupport));
}

void get_extremal_vertex(convex_hull_t* obj, const dvec3& dir, const mtx4x3& mtx, dvec3& support)
{
	vec3 localdir; localdir.set((f32)dir.x,(f32)dir.y,(f32)dir.z);
	localdir=mtx.transformtransposed3x3(localdir);
	vec3 localsupport;
	obj->get_extremal_vertex(localdir,localsupport);
	support=dvec3(mtx.transform(localsupport));
}




gjk_intersection::gjk_intersection(convex_mesh_data_t* i_obj1, convex_mesh_data_t* i_obj2,const mtx4x3& i_mtx1, const mtx4x3& i_mtx2, const dvec3& initdir):
obj1(i_obj1),
obj2(i_obj2),
//mtx1(i_mtx1),
//mtx2(i_mtx2),
simplex_size(0)
{
	out=0;
	double simplex_dist=-FLT_MAX;

	if (initdir.squarelength()==0)
	{
		dvec3 center1(i_mtx1.transform(obj1->center));
		dvec3 center2(i_mtx2.transform(obj2->center));
		dir=center2-center1;
		dir.normalize();
	}
	else
	{
		dir=initdir;
	}
	for (iteration=0; iteration<MAX_ITERATION_COUNT; ++iteration)
	{
		prevdir=dir;
		dvec3 supp,supp1,supp2;

		get_extremal_vertex(obj1,dir,i_mtx1,supp1);
		get_extremal_vertex(obj2,-dir,i_mtx2,supp2);
		supp=supp1-supp2;

		double origo_dist=dot(supp,dir);
		if (origo_dist<0)
		{
			result=false;
			return;
		}

		if (origo_dist<simplex_dist+.00001)//nem tudunk kozelebb menni
		{
			result=true;
			out=1;
			return;
		}

		simplex1[simplex_size]=supp1;
		simplex2[simplex_size]=supp2;
		simplex[simplex_size++]=supp;

		get_min_subsimplex();

		if (simplex_size==4) //megvan a tetraeder, vege
		{
			result=true;
			out=2;
			return;
		}

		if (dir.squarelength()<.000001) //az origo rajta van a simplexen, vege
		{
			result=true;
			out=3;
			return;
		}

		dir.normalize();
		double d=dot(simplex[0],dir);

//		if (max_dist<d)
			simplex_dist=d;
	}

	result=true;
	out=4;
}


void gjk_intersection::get_min_subsimplex()
{
	switch (simplex_size)
	{
	case 1://vertex
		{
			dir=-simplex[0];
			return;
		}
	case 2://segment
		{
			if (origo_in_segmentvertex_region(1,0))
				return;

			dvec3 segment(simplex[1]-simplex[0]);
			dir=(dot(simplex[0],segment)/segment.squarelength())*segment-simplex[0];
			return;
		}
	case 3://triangle
		{
			if (origo_in_trianglevertex_region(2,0,1))	//2. vertex
				return;
			if (origo_in_triangleedge_region(0,2,1))
				return;
			if (origo_in_triangleedge_region(1,2,0))
				return;

			//0-1-2 face
			dir.cross(simplex[2]-simplex[0],simplex[1]-simplex[0]);
			dir*=-dot(dir,simplex[0])/dir.squarelength();
			return;
		}

	case 4://tetrahedron
		{
			if (origo_in_tetrahedronvertex_region(3,0,1,2))
				return;
			if (origo_in_tetrahedronedge_region(0,3,1,2))
				return;
			if (origo_in_tetrahedronedge_region(1,3,0,2))
				return;
			if (origo_in_tetrahedronedge_region(2,3,0,1))
				return;
			if (origo_in_tetrahedronface_region(0,1,3,2))
				return;
			if (origo_in_tetrahedronface_region(0,2,3,1))
				return;
			if (origo_in_tetrahedronface_region(1,2,3,0))
				return;
			return;
		}
	}

	assertion(0);
}


bool gjk_intersection::origo_in_segmentvertex_region(int v1,int v2)
{
	if (dot(simplex[v1],simplex[v2]-simplex[v1])>=0)
	{
		simplex[0]=simplex[v1];
		simplex1[0]=simplex1[v1];
		simplex2[0]=simplex2[v1];
		dir=-simplex[0];
		simplex_size=1;
		return true;
	}
	return false;
}
bool gjk_intersection::origo_in_trianglevertex_region(int v1,int v2,int v3)
{
	if (dot(simplex[v1],simplex[v2]-simplex[v1])>=0 && 
		dot(simplex[v1],simplex[v3]-simplex[v1])>=0)
	{
		simplex[0]=simplex[v1];
		simplex1[0]=simplex1[v1];
		simplex2[0]=simplex2[v1];
		dir=-simplex[0];
		simplex_size=1;
		return true;
	}
	return false;
}
bool gjk_intersection::origo_in_triangleedge_region(int v1,int v2,int v3)
{
	if (dot(simplex[v1],simplex[v2]-simplex[v1])<=0 &&
		dot(simplex[v2],simplex[v1]-simplex[v2])<=0 &&
		dot(simplex[v1],cross(simplex[v2]-simplex[v1],
		cross(	simplex[v2]-simplex[v1],
		simplex[v3]-simplex[v1])))<0)
	{
		simplex[0]=simplex[v1];
		simplex[1]=simplex[v2];
		simplex1[0]=simplex1[v1];
		simplex1[1]=simplex1[v2];
		simplex2[0]=simplex2[v1];
		simplex2[1]=simplex2[v2];
		simplex_size=2;
		dvec3 edge(simplex[1]-simplex[0]);
		dir=(dot(simplex[0],edge)/edge.squarelength())*edge-simplex[0];
		return true;
	}
	return false;
}
bool gjk_intersection::origo_in_tetrahedronvertex_region(int v1,int v2,int v3, int v4)
{
	if (dot(simplex[v1],simplex[v2]-simplex[v1])>=0 &&
		dot(simplex[v1],simplex[v3]-simplex[v1])>=0 &&
		dot(simplex[v1],simplex[v4]-simplex[v1])>=0)
	{
		simplex[0]=simplex[v1];
		simplex1[0]=simplex1[v1];
		simplex2[0]=simplex2[v1];
		dir=-simplex[0];
		simplex_size=1;
		return true;
	}
	return false;
}
bool gjk_intersection::origo_in_tetrahedronedge_region(int v1,int v2,int v3, int v4)
{
	const dvec3 n123=cross(simplex[v2]-simplex[v1],simplex[v3]-simplex[v1]);
	const dvec3 n142=cross(simplex[v4]-simplex[v1],simplex[v2]-simplex[v1]);

	if (dot(simplex[v1],simplex[v2]-simplex[v1])<=0 &&
		dot(simplex[v2],simplex[v1]-simplex[v2])<=0 &&
		dot(simplex[v1],cross(simplex[v2]-simplex[v1],n123))<=0 &&
		dot(simplex[v1],cross(n142,simplex[v2]-simplex[v1]))<=0)
	{
		simplex[0]=simplex[v1];
		simplex[1]=simplex[v2];
		simplex1[0]=simplex1[v1];
		simplex1[1]=simplex1[v2];
		simplex2[0]=simplex2[v1];
		simplex2[1]=simplex2[v2];
		dvec3 edge(simplex[1]-simplex[0]);
		dir=(dot(simplex[0],edge)/edge.squarelength())*edge-simplex[0];
		simplex_size=2;
		return true;
	}

	return false;
}
bool gjk_intersection::origo_in_tetrahedronface_region(int v1,int v2,int v3, int v4)
{
	const dvec3 n123=cross(simplex[v2]-simplex[v1],simplex[v3]-simplex[v1]);
	if (dot(simplex[v1],n123)*dot(simplex[v4]-simplex[v1],n123)<0)
		return false;

	if (dot(simplex[v1],cross(simplex[v2]-simplex[v1],n123))<0)
		return false;

	if (dot(simplex[v1],cross(simplex[v1]-simplex[v3],n123))<0)
		return false;

	const dvec3 n1223=cross(simplex[v3]-simplex[v2],simplex[v1]-simplex[v2]);

	if (dot(simplex[v2],cross(simplex[v3]-simplex[v2],n1223))<0)
		return false;

	simplex[0]=simplex[v1];
	simplex[1]=simplex[v2];
	simplex[2]=simplex[v3];
	simplex1[0]=simplex1[v1];
	simplex1[1]=simplex1[v2];
	simplex1[2]=simplex1[v3];
	simplex2[0]=simplex2[v1];
	simplex2[1]=simplex2[v2];
	simplex2[2]=simplex2[v3];

	dir=cross(simplex[2]-simplex[0],simplex[1]-simplex[0]);
	dir*=-dot(dir,simplex[0])/dir.squarelength();
	simplex_size=3;
	return true;
}

void gjk_intersection::calculate_contact_data()
{

}



























gjk_intersection2::gjk_intersection2(convex_hull_t* i_obj1, convex_hull_t* i_obj2,const mtx4x3& i_mtx1, const mtx4x3& i_mtx2, const dvec3& initdir):
obj1(i_obj1),
obj2(i_obj2),
//mtx1(i_mtx1),
//mtx2(i_mtx2),
simplex_size(0)
{
	out=0;
	double simplex_dist=-FLT_MAX;

	if (initdir.squarelength()==0)
	{
		dvec3 center1(i_mtx1.transform(obj1->center));
		dvec3 center2(i_mtx2.transform(obj2->center));
		dir=center2-center1;
		dir.normalize();
	}
	else
	{
		dir=initdir;
	}
	for (iteration=0; iteration<MAX_ITERATION_COUNT; ++iteration)
	{
		prevdir=dir;
		dvec3 supp,supp1,supp2;

		get_extremal_vertex(obj1,dir,i_mtx1,supp1);
		get_extremal_vertex(obj2,-dir,i_mtx2,supp2);
		supp=supp1-supp2;

		double origo_dist=dot(supp,dir);
		if (origo_dist<0)
		{
			result=false;
			return;
		}

		if (origo_dist<simplex_dist+.00001)//nem tudunk kozelebb menni
		{
			result=true;
			out=1;
			return;
		}

		simplex1[simplex_size]=supp1;
		simplex2[simplex_size]=supp2;
		simplex[simplex_size++]=supp;

		get_min_subsimplex();

		if (simplex_size==4) //megvan a tetraeder, vege
		{
			result=true;
			out=2;
			return;
		}

		double l2=dir.squarelength();
		if (l2<.000001) //az origo rajta van a simplexen, vege
		{
			result=true;
			out=3;
			return;
		}

		dir*=1.0/sqrt(l2);
//		dir.normalize();
		double d=dot(simplex[0],dir);

		//		if (max_dist<d)
		simplex_dist=d;
	}

	result=true;
	out=4;
}


void gjk_intersection2::get_min_subsimplex()
{
	switch (simplex_size)
	{
	case 1://vertex
		{
			dir=-simplex[0];
			return;
		}
	case 2://segment
		{
			if (origo_in_segmentvertex_region(1,0))
				return;

			dvec3 segment(simplex[1]-simplex[0]);
			dir=(dot(simplex[0],segment)/segment.squarelength())*segment-simplex[0];
			return;
		}
	case 3://triangle
		{
			if (origo_in_trianglevertex_region(2,0,1))	//2. vertex
				return;
			if (origo_in_triangleedge_region(0,2,1))
				return;
			if (origo_in_triangleedge_region(1,2,0))
				return;

			//0-1-2 face
			dir.cross(simplex[2]-simplex[0],simplex[1]-simplex[0]);
			dir*=-dot(dir,simplex[0])/dir.squarelength();
			return;
		}

	case 4://tetrahedron
		{
			if (origo_in_tetrahedronvertex_region(3,0,1,2))
				return;
			if (origo_in_tetrahedronedge_region(0,3,1,2))
				return;
			if (origo_in_tetrahedronedge_region(1,3,0,2))
				return;
			if (origo_in_tetrahedronedge_region(2,3,0,1))
				return;
			if (origo_in_tetrahedronface_region(0,1,3,2))
				return;
			if (origo_in_tetrahedronface_region(0,2,3,1))
				return;
			if (origo_in_tetrahedronface_region(1,2,3,0))
				return;
			return;
		}
	}

	assertion(0);
}


bool gjk_intersection2::origo_in_segmentvertex_region(int v1,int v2)
{
	if (dot(simplex[v1],simplex[v2]-simplex[v1])>=0)
	{
		simplex[0]=simplex[v1];
		simplex1[0]=simplex1[v1];
		simplex2[0]=simplex2[v1];
		dir=-simplex[0];
		simplex_size=1;
		return true;
	}
	return false;
}
bool gjk_intersection2::origo_in_trianglevertex_region(int v1,int v2,int v3)
{
	if (dot(simplex[v1],simplex[v2]-simplex[v1])>=0 && 
		dot(simplex[v1],simplex[v3]-simplex[v1])>=0)
	{
		simplex[0]=simplex[v1];
		simplex1[0]=simplex1[v1];
		simplex2[0]=simplex2[v1];
		dir=-simplex[0];
		simplex_size=1;
		return true;
	}
	return false;
}
bool gjk_intersection2::origo_in_triangleedge_region(int v1,int v2,int v3)
{
	if (dot(simplex[v1],simplex[v2]-simplex[v1])<=0 &&
		dot(simplex[v2],simplex[v1]-simplex[v2])<=0 &&
		dot(simplex[v1],cross(simplex[v2]-simplex[v1],
		cross(	simplex[v2]-simplex[v1],
		simplex[v3]-simplex[v1])))<0)
	{
		simplex[0]=simplex[v1];
		simplex[1]=simplex[v2];
		simplex1[0]=simplex1[v1];
		simplex1[1]=simplex1[v2];
		simplex2[0]=simplex2[v1];
		simplex2[1]=simplex2[v2];
		simplex_size=2;
		dvec3 edge(simplex[1]-simplex[0]);
		dir=(dot(simplex[0],edge)/edge.squarelength())*edge-simplex[0];
		return true;
	}
	return false;
}
bool gjk_intersection2::origo_in_tetrahedronvertex_region(int v1,int v2,int v3, int v4)
{
	if (dot(simplex[v1],simplex[v2]-simplex[v1])>=0 &&
		dot(simplex[v1],simplex[v3]-simplex[v1])>=0 &&
		dot(simplex[v1],simplex[v4]-simplex[v1])>=0)
	{
		simplex[0]=simplex[v1];
		simplex1[0]=simplex1[v1];
		simplex2[0]=simplex2[v1];
		dir=-simplex[0];
		simplex_size=1;
		return true;
	}
	return false;
}
bool gjk_intersection2::origo_in_tetrahedronedge_region(int v1,int v2,int v3, int v4)
{
	const dvec3 n123=cross(simplex[v2]-simplex[v1],simplex[v3]-simplex[v1]);
	const dvec3 n142=cross(simplex[v4]-simplex[v1],simplex[v2]-simplex[v1]);

	if (dot(simplex[v1],simplex[v2]-simplex[v1])<=0 &&
		dot(simplex[v2],simplex[v1]-simplex[v2])<=0 &&
		dot(simplex[v1],cross(simplex[v2]-simplex[v1],n123))<=0 &&
		dot(simplex[v1],cross(n142,simplex[v2]-simplex[v1]))<=0)
	{
		simplex[0]=simplex[v1];
		simplex[1]=simplex[v2];
		simplex1[0]=simplex1[v1];
		simplex1[1]=simplex1[v2];
		simplex2[0]=simplex2[v1];
		simplex2[1]=simplex2[v2];
		dvec3 edge(simplex[1]-simplex[0]);
		dir=(dot(simplex[0],edge)/edge.squarelength())*edge-simplex[0];
		simplex_size=2;
		return true;
	}

	return false;
}
bool gjk_intersection2::origo_in_tetrahedronface_region(int v1,int v2,int v3, int v4)
{
	const dvec3 n123=cross(simplex[v2]-simplex[v1],simplex[v3]-simplex[v1]);
	if (dot(simplex[v1],n123)*dot(simplex[v4]-simplex[v1],n123)<0)
		return false;

	if (dot(simplex[v1],cross(simplex[v2]-simplex[v1],n123))<0)
		return false;

	if (dot(simplex[v1],cross(simplex[v1]-simplex[v3],n123))<0)
		return false;

	const dvec3 n1223=cross(simplex[v3]-simplex[v2],simplex[v1]-simplex[v2]);

	if (dot(simplex[v2],cross(simplex[v3]-simplex[v2],n1223))<0)
		return false;

	simplex[0]=simplex[v1];
	simplex[1]=simplex[v2];
	simplex[2]=simplex[v3];
	simplex1[0]=simplex1[v1];
	simplex1[1]=simplex1[v2];
	simplex1[2]=simplex1[v3];
	simplex2[0]=simplex2[v1];
	simplex2[1]=simplex2[v2];
	simplex2[2]=simplex2[v3];

	dir=cross(simplex[2]-simplex[0],simplex[1]-simplex[0]);
	dir*=-dot(dir,simplex[0])/dir.squarelength();
	simplex_size=3;
	return true;
}

void gjk_intersection2::calculate_contact_data()
{

}