#ifndef _convexmeshshape_h_
#define _convexmeshshape_h_

#include "shape.h"
#include "convexmeshdata.h"

//////////////////////////////////////////////////////////////////////////
//		convex mesh shape
//////////////////////////////////////////////////////////////////////////


struct convex_mesh_shape_desc: shape_desc
{
	convex_mesh_shape_desc()
	{
		type=shape_type_convex_mesh;
	}
	convex_mesh_data_t* mesh_data;
};

struct convex_mesh_shape_t: shape_t
{
	convex_mesh_shape_t(const convex_mesh_shape_desc& desc);

	vec3 get_center() const {return mesh_data->center;}
	void get_extreme_point(vec3& p, const vec3& dir);


	convex_mesh_data_t* mesh_data;
};



inline convex_mesh_shape_t::convex_mesh_shape_t(const convex_mesh_shape_desc& desc)
{
	type=shape_type_convex_mesh;
	mesh_data=desc.mesh_data;

	bounding.min=mesh_data->center-mesh_data->half_extent;
	bounding.max=mesh_data->center+mesh_data->half_extent;
}

inline void convex_mesh_shape_t::get_extreme_point(vec3& p, const vec3& dir)
{
	get_extremal_vertex(mesh_data,dir,p);
}
#endif//convexmeshshape_h_