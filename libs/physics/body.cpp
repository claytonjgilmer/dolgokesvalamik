#include "body.h"
#include "physicssystem.h"
#include "utils/assert.h"

namespace physics
{
	body_t::body_t()
	{
		this->contacts=NULL;
		this->joints=NULL;
	}

	body_t::~body_t()
	{
		while (this->contacts)
			system::instance()->cm.erase_contact((contact_t*)this->contacts->constraint);
	}

	const math::mtx4x3& body_t::get_pos() const
	{
		utils::assertion(array_index!=-1);
		return system::instance()->bodystate_array[is_static].pos[array_index];
	}

	const math::vec3& body_t::get_vel() const
	{
		utils::assertion(array_index!=-1);
		return system::instance()->bodystate_array[is_static].vel[array_index];
	}

	const math::vec3& body_t::get_rotvel() const
	{
		utils::assertion(array_index!=-1);
		return system::instance()->bodystate_array[is_static].rotvel[array_index];
	}


	void body_t::set_pos(const math::mtx4x3& i_pos)
	{
		utils::assertion(array_index!=-1);
		system::instance()->bodystate_array[is_static].pos[array_index]=i_pos;
	}

	void body_t::set_vel(const math::vec3& i_vel)
	{
		utils::assertion(array_index!=-1);
		system::instance()->bodystate_array[is_static].vel[array_index]=i_vel;
	}

	void body_t::set_rotvel(const math::vec3& i_rotvel)
	{
		utils::assertion(array_index!=-1);
		system::instance()->bodystate_array[is_static].rotvel[array_index]=i_rotvel;
	}





}//namespace