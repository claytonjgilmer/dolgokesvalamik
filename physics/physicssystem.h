#ifndef _physicssystem_h_
#define _physicssystem_h_

#include "../containers/listallocator.h"
#include "../math/mtx4x3.h"
#include "../physics/body.h"

namespace physics
{
	class systemdesc
	{
	public:
	};

/*
	class bodystate
	{
	public:
		math::mtx4x3  m_pos;
		math::vec3	m_vel;
		math::vec3	m_rotvel;

		math::vec3	m_force;
		math::vec3	m_torque;

		float	m_invmass;
		math::mtx3x3	m_invinertiarel;
		math::mtx3x3	m_invinertiaabs;

		float	m_veldamp;
		float	m_rotveldamp;
		body*	m_body;
	};
*/

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

	template<unsigned blocksize=512>
	struct bodystates
	{
		ctr::vector<nbody<blocksize>*> m_blocks;

		unsigned m_bodynum;

		bodystates()
		{
			m_bodynum=0;

			newblock();

		}

		~bodystates()
		{
			for (unsigned n=0; n<m_blocks.size(); delete m_blocks[n++]);
		}

		unsigned create(const bodydesc& i_desc, body* i_body)
		{
			if (m_bodynum==m_blocks.size()*blocksize)
				newblock();

			unsigned index(m_bodynum & (blocksize-1));
			nbody<blocksize>* last=m_blocks.back();

			last->m_pos[index]=i_desc.m_pos;
			last->m_vel[index]=i_desc.m_vel;
			last->m_rotvel[index]=i_desc.m_rotvel;
			last->m_body[index]=i_body;

			return m_bodynum++;
		}

		body* release(unsigned i_index)
		{
			--m_bodynum;

			nbody<blocksize>* dst=m_blocks[i_index >> (math::logn<blocksize>())];
			unsigned dstindex=i_index & (blocksize-1);
			nbody<blocksize>* src=m_blocks.back();
			unsigned srcindex=m_bodynum & (blocksize-1);
			dst->m_pos[dstindex]=src->m_pos[srcindex];
			dst->m_vel[dstindex]=src->m_vel[srcindex];
			dst->m_rotvel[dstindex]=src->m_rotvel[srcindex];
			dst->m_body[dstindex]=src->m_body[srcindex];

			return dst->m_body[dstindex];
		}


	private:
		void newblock()
		{
			m_blocks.push_back(new nbody<blocksize>);
		}
	};




	class system
	{
	public:
		static system* createsystem(const systemdesc& i_desc)
		{
			return new system(i_desc);
		}

		void release()
		{
			delete this;
		}

		body* createbody(const bodydesc& i_desc)
		{
			body* b=new body();

			b->m_index=m_bodystates.create(i_desc,b);

			return b;
		}

		void releasebody(body* i_body)
		{
			unsigned index=i_body->m_index;
			m_bodystates.release(index)->m_index=index;

			delete i_body;
		}

		void simulate(float i_dt);

	private:
		system(const systemdesc& i_sd)
		{
		}

		~system()
		{
		}

		bodystates<> m_bodystates;
	};
}
#endif//_physicssystem_h_