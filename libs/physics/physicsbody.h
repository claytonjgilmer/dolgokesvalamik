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

	class body_t
	{
		friend class system;
	private:
		int m_index;
	};

#endif