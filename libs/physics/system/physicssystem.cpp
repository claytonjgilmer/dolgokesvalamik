#include "physicssystem.h"
#include "threading/taskmanager.h"

DEFINE_SINGLETON(physicssystem);

void kill_deads();
void update_inertia();
void update_bodies(float dt);
void broadphase();
void near_phase();
void create_contact_groups();
void update_contacts();


physicssystem::physicssystem(const physicssystemdesc* i_desc):
desc(*i_desc)
{
    ZeroMemory(this->intersect_fn,sizeof(this->intersect_fn));
    this->intersect_fn[shape_type_sphere][shape_type_sphere]=&test_sphere_sphere_intersect;
    this->parallel_processing=TRUE;
}

body_t* physicssystem::create_body(const bodydesc& i_desc)
{
    body_t* b=this->body_list.allocate();
    b->is_static=i_desc.is_static;

    this->bodystate_array[i_desc.is_static].add_body(&i_desc,&b,1);

    return b;
}

void physicssystem::release_body(body_t* i_body)
{
    killed[i_body->is_static].push_back(i_body);
}

void physicssystem::create_bodies(body_t* o_body_array[], bodystate i_statics, const bodydesc i_desc[], unsigned i_bodynum)
{
    for (unsigned n=0; n<i_bodynum;++n)
        (o_body_array[n]=this->body_list.allocate_place())->is_static=i_statics;

    this->bodystate_array[i_statics].add_body(i_desc,o_body_array,i_bodynum);
}

void physicssystem::release_bodies(body_t* i_body_array[], bodystate i_state, unsigned i_bodynum)
{
    for (uint32 n=0; n<i_bodynum; ++n)
        this->killed[i_state].push_back(i_body_array[n]);
}

void physicssystem::simulate(float i_dt)
{
    kill_deads();

    broadphase();
    near_phase();
    update_contacts();
    create_contact_groups();
    update_inertia();
    update_bodies(i_dt);
}


void kill_deads()
{
    physicssystem* sys=physicssystem::ptr;

    if (sys->killed[0].size())
    {
        sys->bodystate_array[0].release_body(&*sys->killed[0].begin(),sys->killed[0].size());
        for (uint32 n=0; n<sys->killed[0].size(); ++n)
            sys->body_list.deallocate(sys->killed[0][n]);
    }
    if (sys->killed[1].size())
    {
        sys->bodystate_array[1].release_body(&*sys->killed[1].begin(),sys->killed[1].size());
        for (uint32 n=0; n<sys->killed[1].size(); ++n)
            sys->body_list.deallocate(sys->killed[1][n]);
    }

}


struct update_bounding
{

    void operator()(unsigned i_start,unsigned i_num) const
    {
		const nbody& nb=physicssystem::ptr->bodystate_array[BODYSTATE_DYNAMIC];
        uint32 end=i_start+i_num;
        for (uint32 n=i_start; n<end; ++n)
        {
            intr_list::iterator it,endit;
            endit=nb.body[n]->shapes.end();
            for (it=nb.body[n]->shapes.begin(); it!=endit; ++it)
            {
                shape_t* s=(shape_t*)(*it);
				s->collider->bounding_world=transform(nb.pos[n],s->bounding);
            }
        }
    }
};


void broadphase()
{
    physicssystem* ptr=physicssystem::ptr;
    const nbody& nb=ptr->bodystate_array[BODYSTATE_DYNAMIC];

    if (ptr->parallel_processing)
    {
        taskmanager::ptr->process_buffer(nb.size,10,update_bounding());
    }
    else
    {
        update_bounding ub;
        ub(0,nb.size);
    }

    ptr->broad_phase.update();
}

struct near_struct
{
    void operator()(unsigned i_start,unsigned i_num) const
    {
        physicssystem* const ptr=physicssystem::ptr;
        const broadphasepair* array=ptr->broad_phase.pair_array;
        const uint32 end=i_start+i_num;
        for (uint32 n=i_start; n<end; ++n)
        {
            shape_t* shape1=(shape_t*)array[n].object[0]->userdata;
            shape_t* shape2=(shape_t*)array[n].object[1]->userdata;

            assert(shape1!=shape2);

            assertion(shape1!=shape2);

            if (shape1->body!=shape2->body)
            {
                if (ptr->intersect_fn[shape1->type][shape2->type])
                {
                    vec3 contact[20][2];
                    vec3 normal;
                    unsigned contact_count;
                    const mtx4x3& body1_mtx=ptr->bodystate_array[shape1->body->is_static].pos[shape1->body->array_index];
                    const mtx4x3& body2_mtx=ptr->bodystate_array[shape2->body->is_static].pos[shape2->body->array_index];
                    int intersect=ptr->intersect_fn[shape1->type][shape2->type](shape1,body1_mtx,
																				shape2,body2_mtx,
																				contact,
																				normal,
																				contact_count);

                    if (intersect)
                    {
/*
                        vec3 vel1=shape1->body->get_vel();
                        vec3 vel2=shape2->body->get_vel();

                        float velmag=0.5f*dot(vel2-vel1,normal);

                        if (velmag<0)
                        {
                            shape1->body->set_vel(vel1+velmag*normal);
                            shape2->body->set_vel(vel2-velmag*normal);
                        }
*/
                        contact_t* c=ptr->contact_manager.get_contact(shape1->body,shape2->body);

                        c->normal=normal;
                        vec3 normal_body1; body1_mtx.transformtransposed3x3(normal_body1,normal);
						c->add_contact(contact,contact_count,normal_body1);
                    }
                }
            }
        }
    }
};

void near_phase()
{
    physicssystem* ptr=physicssystem::ptr;

    if (ptr->parallel_processing)
    {
		if (ptr->broad_phase.pair_num)
			taskmanager::ptr->process_buffer(ptr->broad_phase.pair_num,10,near_struct());
    }
    else
    {
        near_struct ns;
        ns(0,ptr->broad_phase.pair_num);
    }
}

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

void update_bodies(float i_dt)
{
    physicssystem* ptr=physicssystem::ptr;
    nbody* b=ptr->bodystate_array+BODYSTATE_DYNAMIC;
    if (ptr->parallel_processing)
    {
        taskmanager::ptr->process_buffer(b->size,10,update_process(b,i_dt,ptr->desc.gravity));
    }
    else
    {
        update_process p(b,i_dt,ptr->desc.gravity);
        p(0,b->size);
    }
}

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

void update_inertia()
{
    physicssystem* ptr=physicssystem::ptr;
    nbody* b=ptr->bodystate_array+BODYSTATE_DYNAMIC;

    if (ptr->parallel_processing)
    {
        taskmanager::ptr->process_buffer(b->size,10,inertia_process(b));
    }
    else
    {
        inertia_process i(b);
        i(0,b->size);
    }
}


void create_contact_groups()
{
    physicssystem* ptr=physicssystem::ptr;
    ptr->contact_group_manager.create_contact_groups(ptr->bodystate_array[BODYSTATE_DYNAMIC].body,
                                                    ptr->bodystate_array[BODYSTATE_DYNAMIC].size,
                                                    ptr->contact_manager.contact_list);
}

void update_contacts()
{
    physicssystem* ptr=physicssystem::ptr;
    ptr->contact_manager.update_contacts();
}
