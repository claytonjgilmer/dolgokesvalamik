#ifndef _PHYSICSBODY_H_
#define _PHYSICSBODY_H_

namespace physics
{
	class system;
	struct constraint_edge;

	struct bodydesc
	{
		math::mtx4x3 pos;
		math::vec3 vel;
		math::vec3 rotvel;

		float		mass;
		math::mtx3x3 inertia;

		BOOL		is_static;

		bodydesc()
		{
			pos.identity();
			vel.clear();
			rotvel.clear();
			mass=1.0f;
			inertia.identity();
			is_static=FALSE;
		}
	};

	struct body_t
	{
		int		array_index;
		bool	is_static;

	};
}
#endif