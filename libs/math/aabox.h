#ifndef _aabox_h_
#define _aabox_h_

#include "vec3.h"

namespace math
{
	struct aabb
	{
		math::vec3 min;
		math::vec3 max;
	};
}
#endif//_aabox_h_