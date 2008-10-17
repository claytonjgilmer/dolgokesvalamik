#include "physicssystem.h"

namespace physics
{
	DEFINE_SINGLETON(system);


	system::system(const systemdesc* i_desc):
	desc(*i_desc)
	{
	}

	body_t* system::create_body(const bodydesc& i_desc)
	{
		body_t* b=body_list.allocate_place();
		b->is_static=i_desc.is_static;

		bodystate_array[i_desc.is_static].add_body(&i_desc,&b,1);

		return b;
	}

	void system::release_body(body_t* i_body)
	{
		killed[i_body->is_static].push_back(i_body);
	}

	void system::create_bodies(body_t* o_body_array[], bodystate i_statics, const bodydesc i_desc[], unsigned i_bodynum)
	{
		for (unsigned n=0; n<i_bodynum;++n)
			(o_body_array[n]=body_list.allocate_place())->is_static=i_statics;

		bodystate_array[i_statics].add_body(i_desc,o_body_array,i_bodynum);
	}

	void system::release_bodies(body_t* i_body_array[], bodystate i_state, unsigned i_bodynum)
	{
		for (uint32 n=0; n<i_bodynum; ++n)
		{
			killed[i_state].push_back(i_body_array[n]);
		}
	}

	void system::simulate(float i_dt)
	{
		if (killed[0].size())
		{
			bodystate_array[0].release_body(&*killed[0].begin(),killed[0].size());
			for (uint32 n=0; n<killed[0].size(); ++n)
				body_list.deallocate(killed[0][n]);
		}
		if (killed[1].size())
		{
			bodystate_array[1].release_body(&*killed[1].begin(),killed[1].size());
			for (uint32 n=0; n<killed[1].size(); ++n)
				body_list.deallocate(killed[1][n]);
		}

		update_inertia();
		update_bodies(i_dt);
	}

	void system::update_bodies(float i_dt)
	{
		nbody* b=bodystate_array+BODYSTATE_DYNAMIC;

		for (uint32 n=0; n<b->size;++n)
		{
			b->vel[n]+=i_dt*b->invmass[n]*b->force[n];
			b->rotvel[n]+=i_dt*b->invinertia_abs[n].transform3x3(b->torque[n]);

			b->pos[n].t+=i_dt*b->vel[n];

			float angle=b->rotvel[n].length();

			if (angle>0.001f)
			{
				math::vec3 axis=b->rotvel[n]/angle;
				b->pos[n].rotate(b->pos[n],axis,angle);
			}
		}
	}

	void system::update_inertia()
	{
		nbody* b=bodystate_array+BODYSTATE_DYNAMIC;
		for (uint32 n=0; n<b->size; ++n)
		{
			math::mtx3x3 invpos; invpos.transpose3x3(b->pos[n]);
			math::mtx3x3 tmp; tmp.multiply3x3(invpos,b->invinertia_rel[n]);
			b->invinertia_abs[n].multiply3x3(tmp,b->pos[n]);
		}
	}
}//namespace physics