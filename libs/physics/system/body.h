#ifndef _PHYSICSBODY_H_
#define _PHYSICSBODY_H_

#include "math/mtx4x3.h"

namespace physics
{
	class system;
	struct constraint_edge;

	enum bodystate
	{
		BODYSTATE_DYNAMIC,
		BODYSTATE_STATIC
	};

	struct bodydesc
	{
		math::mtx4x3	pos;
		math::vec3		vel;
		math::vec3		rotvel;

		float			mass;
		math::mtx3x3	inertia;

		bodystate		is_static;

		bodydesc()
		{
			pos.identity();
			vel.clear();
			rotvel.clear();
			mass=1.0f;
			inertia.identity();
			is_static=BODYSTATE_DYNAMIC;
		}
	};

	struct body_t
	{
		const math::mtx4x3& get_pos() const;
		const math::vec3& get_vel() const;
		const math::vec3& get_rotvel() const;

		void set_pos(const math::mtx4x3& i_pos);
		void set_vel(const math::vec3& i_vel);
		void set_rotvel(const math::vec3& i_rotvel);


		body_t();
		~body_t();

		uint16 array_index;
		bodystate  is_static;
		constraint_edge* contacts;
		constraint_edge* joints;

	};
}
#endif