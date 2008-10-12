#include "physicssystem.h"

namespace physics
{
	DEFINE_SINGLETON(system);
/*
	system::system(const systemdesc* i_desc):
	desc(*i_desc)
	{
	}

	inline void system::substep();

	void system::simulate(float i_dt)
	{
		unsigned substepnum=(unsigned)(i_dt/this->desc.timestep);
		float dt=this->desc.timestep*(float)substepnum;

		for (unsigned n=0; n<substepnum;++n)
		{
			this->substep(dt);
		}
	}

	inline void system::predict(float i_dt)
	{
		velocity+=i_dt*invmass*force;
		angularaccel=ininertiaabs*torque;
		rotvel+=i_dt*angularaccel;
	}

	inline void system::collide()
	{
		broadphase->update();
		collide_shapepairs();
	}

	inline void system::substep(float i_dt)
	{
		this->predict(i_dt);
		this->collide();
		this->create_contactgroups();
		this->solve_constraints(i_dt);
		this->update_bodies(i_dt);
	}
*/
	body_t* system::create_body(const bodydesc& i_desc)
	{
		body_t* b=body_list.allocate_place();
		bodystate_array.add_body(&i_desc,&b,1);
	}
}//namespace physics