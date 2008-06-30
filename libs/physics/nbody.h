#ifndef _nbody_h_
#define _nbody_h_

namespace physics
{
	template<unsigned N=512> class nbody
	{
	public:
		math::mtx4x3	m_pos[N];
		math::vec3		m_vel[N];
		math::vec3		m_rotvel[N];

		math::vec3		m_force[N];
		math::vec3		m_torque[N];

		float			m_invmass[N];
		math::mtx3x3	m_invinertiarel[N];
		math::mtx3x3	m_invinertiaabs[N];

		float			m_veldamp[N];
		float			m_rotveldamp[N];
		body*			m_body[N];
	};
}
#endif//_nbody_h_