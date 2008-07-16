#ifndef _PHYSICSBODY_H_
#define _PHYSICSBODY_H_

namespace physics
{
	class system;

	struct bodydesc
	{
		math::mtx4x3 m_pos;
		math::vec3 m_vel;
		math::vec3 m_rotvel;

		bodydesc()
		{
			m_pos.identity();
			m_vel.clear();
			m_rotvel.clear();
		}
	};

	class body
	{
		friend class system;
	private:
		body(){}
		~body(){}
		int m_index;
	};
}
#endif