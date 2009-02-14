#ifndef _broadphaseobject_h_
#define _broadphaseobject_h_

#include "math/aabox.h"
	struct broadphaseobject
	{
		broadphaseobject(void* i_userdata, const aabb_t& i_bounding_world, uint32 i_is_static):
		userdata(i_userdata),
		bounding_world(i_bounding_world),
		is_static(i_is_static)
		{
		}


		void*		userdata;
		aabb_t	bounding_world;
		uint32		is_static;
	};
#endif//_broadphaseobject_h_
