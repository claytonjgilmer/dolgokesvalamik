#include "physicssystem.h"

namespace physics
{
	DEFINE_SINGLETON(system);
	body_t* system::create_body(const bodydesc& i_desc)
	{
		body_t* b=body_list.allocate_place();
		b->is_static=i_desc.is_static;

		bodystate_array[i_desc.is_static].add_body(&i_desc,&b,1);

		return b;
	}

	void system::release_body(body_t* i_body)
	{
		killed.push_back(i_body);
		i_body->array_index=-1;
	}

	void system::create_bodies(body_t* o_body_array[], UINT8 i_statics, const bodydesc i_desc[], unsigned i_bodynum)
	{
		for (unsigned n=0; n<i_bodynum;++n)
			(o_body_array[n]=body_list.allocate_place())->is_static=i_statics;

		bodystate_array[i_statics].add_body(i_desc,o_body_array,i_bodynum);
	}
}//namespace physics