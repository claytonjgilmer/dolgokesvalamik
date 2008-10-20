#ifndef _broadphaseobject_h_
#define _broadphaseobject_h_

#include "math/aabox.h"
namespace physics
{
	struct broadphaseobject
	{
		broadphaseobject(void* i_userdata, const math::aabb& i_bounding_world, uint32 i_is_static):
		userdata(i_userdata),
		bounding_world(i_bounding_world),
		is_static(i_is_static)
		{
		}


		void*		userdata;
		math::aabb	bounding_world;
		uint32		is_static;
	};
} //namespace Dyn
#endif//_broadphaseobject_h_