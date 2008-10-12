#ifndef _PHYSICSBODY_H_
#define _PHYSICSBODY_H_

namespace physics
{
	class system;

	struct bodydesc
	{
		math::mtx4x3 pos;
		math::vec3 vel;
		math::vec3 rotvel;

		float		mass;
		math::mtx3x3 inertia;

		BOOL		isstatic;

		bodydesc()
		{
			pos.identity();
			vel.clear();
			rotvel.clear();
			mass=1.0f;
			inertia.identity();
			isstatic=FALSE;
		}
	};

	class body_t
	{
		friend class system;
	private:
		body_t()
		{

		}
		~body_t(){}
		int index;
	};
}
#endif