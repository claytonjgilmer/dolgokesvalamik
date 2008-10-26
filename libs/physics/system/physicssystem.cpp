#include "physicssystem.h"
#include "threading/taskmanager2.h"

	DEFINE_SINGLETON(physicssystem);

	void update_inertia(physicssystem* i_system);
	void update_bodies(physicssystem* i_system, float i_dt);
	void collide(physicssystem* i_system);




	physicssystem::physicssystem(const physicssystemdesc* i_desc):
	desc(*i_desc)
	{
		ZeroMemory(intersect_fn,sizeof(intersect_fn));
		intersect_fn[shape_type_sphere][shape_type_sphere]=&test_sphere_sphere_intersect;
		parallel_processing=TRUE;
	}

	body_t* physicssystem::create_body(const bodydesc& i_desc)
	{
		body_t* b=body_list.allocate();
		b->is_static=i_desc.is_static;

		bodystate_array[i_desc.is_static].add_body(&i_desc,&b,1);

		return b;
	}

	void physicssystem::release_body(body_t* i_body)
	{
		killed[i_body->is_static].push_back(i_body);
	}

	void physicssystem::create_bodies(body_t* o_body_array[], bodystate i_statics, const bodydesc i_desc[], unsigned i_bodynum)
	{
		for (unsigned n=0; n<i_bodynum;++n)
			(o_body_array[n]=body_list.allocate_place())->is_static=i_statics;

		bodystate_array[i_statics].add_body(i_desc,o_body_array,i_bodynum);
	}

	void physicssystem::release_bodies(body_t* i_body_array[], bodystate i_state, unsigned i_bodynum)
	{
		for (uint32 n=0; n<i_bodynum; ++n)
		{
			killed[i_state].push_back(i_body_array[n]);
		}
	}

	void physicssystem::simulate(float i_dt)
	{
		if (this->killed[0].size())
		{
			bodystate_array[0].release_body(&*killed[0].begin(),killed[0].size());
			for (uint32 n=0; n<killed[0].size(); ++n)
				body_list.deallocate(killed[0][n]);
		}
		if (this->killed[1].size())
		{
			bodystate_array[1].release_body(&*killed[1].begin(),killed[1].size());
			for (uint32 n=0; n<killed[1].size(); ++n)
				body_list.deallocate(killed[1][n]);
		}

		const nbody& nb=this->bodystate_array[BODYSTATE_DYNAMIC];

		for (unsigned n=0; n<nb.size; ++n)
		{
			body_t* b=nb.body[n];

			for (shape_t* s=b->shapes;s;s=s->next)
				s->collider->bounding_world=transform(nb.pos[n],s->bounding);
		}

		this->broad_phase.update();

		collide(this);
		update_inertia(this);
		update_bodies(this,i_dt);
	}

	void collide(physicssystem* i_system)
	{
		return;
		const broadphasepair* array=i_system->broad_phase.pair_array;
		const unsigned pair_num=i_system->broad_phase.pair_num;

		for (unsigned n=0; n<pair_num; ++n)
		{
			shape_t* shape1=(shape_t*)array[n].object[0]->userdata;
			shape_t* shape2=(shape_t*)array[n].object[1]->userdata;

			if (shape1->body!=shape2->body)
				if (i_system->intersect_fn[shape1->type][shape2->type])
				{
					vec3 contact[20][2];
					vec3 normal;
					unsigned contactnum;
					int intersect=i_system->intersect_fn[shape1->type][shape2->type](shape1,i_system->bodystate_array[shape1->body->is_static].pos[shape1->body->array_index],shape2,i_system->bodystate_array[shape2->body->is_static].pos[shape2->body->array_index],contact,normal,contactnum);

					if (intersect)
					{

					}
				}
		}
	}

	void update_bodies(physicssystem* i_system, float i_dt)
	{
		if (i_system->parallel_processing)
		{
			struct update_process
			{
				update_process(nbody* i_b,float i_dt,vec3 i_gravity):
			b(i_b),
			dt(i_dt),
			gravity(i_gravity)
			{
			}

			void operator()(unsigned i_start,unsigned i_num) const
			{
				uint32 end=i_start+i_num;
				for (uint32 n=i_start; n<end; ++n)
				{
					b->vel[n]+=dt*(b->invmass[n]*b->force[n]+gravity);
					b->rotvel[n]+=dt*b->invinertia_abs[n].transform3x3(b->torque[n]);

					b->pos[n].t+=dt*b->vel[n];

					float angle=b->rotvel[n].length();

					if (angle>0.001f)
					{
						vec3 axis=b->rotvel[n]/angle;
						b->pos[n].rotate(b->pos[n],axis,dt*angle);
					}
				}
			}

			nbody* b;
			float dt;
			vec3 gravity;
			};
			nbody* b=i_system->bodystate_array+BODYSTATE_DYNAMIC;
			taskmanager::ptr()->process_buffer(b->size,10,update_process(b,i_dt,i_system->desc.gravity));
		}
		else
		{
			nbody* b=i_system->bodystate_array+BODYSTATE_DYNAMIC;
			for (uint32 n=0; n<b->size;++n)
			{
				b->vel[n]+=i_dt*(b->invmass[n]*b->force[n]+i_system->desc.gravity);
				b->rotvel[n]+=i_dt*b->invinertia_abs[n].transform3x3(b->torque[n]);

				b->pos[n].t+=i_dt*b->vel[n];

				float angle=b->rotvel[n].length();

				if (angle>0.001f)
				{
					vec3 axis=b->rotvel[n]/angle;
					b->pos[n].rotate(b->pos[n],axis,i_dt*angle);
				}
			}
		}
	}

	void update_inertia(physicssystem* i_system)
	{
		nbody* b=i_system->bodystate_array+BODYSTATE_DYNAMIC;

		if (i_system->parallel_processing)
		{
			struct inertia_process
			{
				inertia_process(nbody* i_b):
				b(i_b)
				{
				}

				void operator()(unsigned i_start,unsigned i_num) const
				{
					uint32 end=i_start+i_num;
					for (uint32 n=i_start; n<end; ++n)
					{
						mtx3x3 invpos; invpos.transpose3x3(b->pos[n]);
						mtx3x3 tmp; tmp.multiply3x3(invpos,b->invinertia_rel[n]);
						b->invinertia_abs[n].multiply3x3(tmp,b->pos[n]);
					}
				}

				nbody* b;
			};

			taskmanager::ptr()->process_buffer(b->size,10,inertia_process(b));
		}
		else
		{
			for (uint32 n=0; n<b->size; ++n)
			{
				mtx3x3 invpos; invpos.transpose3x3(b->pos[n]);
				mtx3x3 tmp; tmp.multiply3x3(invpos,b->invinertia_rel[n]);
				b->invinertia_abs[n].multiply3x3(tmp,b->pos[n]);
			}
		}
	}
