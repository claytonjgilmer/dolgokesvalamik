#include "physicssystem.h"

namespace physics
{
	DEFINE_SINGLETON(system);
	body_t* system::create_body(const bodydesc& i_desc)
	{
		body_t* b=body_list.allocate_place();
		b->is_static=i_desc.is_static;
		bodystate_array.add_body(&i_desc,&b,1);
	}
}//namespace physics