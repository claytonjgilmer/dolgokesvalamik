#ifndef _PHYSICSBODY_H_
#define _PHYSICSBODY_H_

namespace physics
{
/*
	class bodystate
	{
	public:
		mtx4x3  m_pos;
//		vec3	m_trans;
//		vec3	m_rot;
		vec3	m_vel;
		vec3	m_rotvel;
		
		vec3	m_force;
		vec3	m_torque;
		
		float	m_invmass;
		mtx3x3	m_invinertiarel;
		mtx3x3	m_invinertiaabs;
		
		float	m_veldamp;
		float	m_rotveldamp;
	};
*/

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