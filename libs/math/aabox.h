#ifndef _aabox_h_
#define _aabox_h_

#include "vec3.h"

namespace math
{
	struct aabb
	{
		vec3 min;
		vec3 max;

		vec3 get_center() const
		{
			return 0.5f*(this->min+this->max);
		}

		vec3 get_extent() const
		{
			return 0.5f*(this->max-this->min);
		}
	};
}
#endif//_aabox_h_