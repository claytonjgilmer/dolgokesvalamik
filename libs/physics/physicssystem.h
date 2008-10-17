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
		float timestep;
		uint8 maxsubstepcount;

		systemdesc()
		{
			gravity=-10;
			timestep=0.01f;
			maxsubstepcount=10;
		}
	};



	class system
	{
		DECLARE_SINGLETON_DESC(system,systemdesc);
	public:
		//body krealas/megszuntetes
		body_t* create_body(const bodydesc& i_desc);
		void create_bodies(body_t* o_body_array[], bodystate i_state, const bodydesc i_desc[], unsigned i_bodynum);
		void release_body(body_t*);
		void release_bodies(body_t* i_body_array[], bodystate i_state, unsigned i_bodynum);


		void simulate(float i_dt);

//	private:
		system(const systemdesc* i_desc);
		void update_inertia();
		void update_bodies(float i_dt);

		systemdesc desc;
		nbody bodystate_array[2];

		ctr::listallocator<body_t> body_list;
		ctr::vector<body_t*> killed[2];
	};
}
#endif//_physicssystem_h_