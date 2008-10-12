#ifndef _physicssystem_h_
#define _physicssystem_h_

#include "utils/singleton.h"
#include "containers/listallocator.h"
#include "math/mtx4x3.h"
#include "physics/body.h"
#include "nbody.h"

namespace physics
{
	struct systemdesc
	{
		float gravity;
		unsigned maxsubstepcount;
		float timestep;
	};



	class system
	{
		DECLARE_SINGLETON_DESC(system,systemdesc);
	public:
		//body krealas/megszuntetes
		body_t* create_body(const bodydesc& i_desc);
		void create_bodies(body_t* & o_body_array[], const bodydesc i_desc[], unsigned i_bodynum);
		void release_body(body_t*);
		void release_bodies(body_t* i_body_array[], unsigned i_bodynum);


		void simulate(float i_dt);

	private:
		system(const systemdesc* i_desc);

		systemdesc desc;
		nbody bodystate_array;

		ctr::listallocator<body_t> body_list;
	};
}
#endif//_physicssystem_h_