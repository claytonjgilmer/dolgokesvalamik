#include "physicssystem.h"
#include "threading/taskmanager.h"
#include "utils/timer.h"

DEFINE_SINGLETON(physicssystem);

void kill_deads();
void update_inertia();
void update_bodies(float dt);
void broadphase();
void near_phase();
void create_contact_groups();
void update_contacts();

body_t* g_world=NULL;


void create_world_body(physicssystem* ptr)
{
	g_world=new (ptr->body_list.allocate_place()) body_t;
	ptr->bodystate_array.add_world();
}

physicssystem::physicssystem(const physicssystemdesc* i_desc):
desc(*i_desc)
{
    ZeroMemory(this->intersect_fn,sizeof(this->intersect_fn));
    this->intersect_fn[shape_type_sphere][shape_type_sphere]=&test_sphere_sphere_intersect;
    this->parallel_boudingupdate=
		parallel_broadphase=
		parallel_nearphase=
		parallel_update=
		parallel_inertia=
		parallel_pre_step=
		parallel_init_accel=
		i_desc->parallel_processing;
	this->frame_count=0;
	this->solver_position_correction_rate=i_desc->solver_positioncorrection_rate;

	create_world_body(this);
}

body_t* physicssystem::create_body(const bodydesc& i_desc)
{
    body_t* b= new (this->body_list.allocate_place()) body_t;
//    b->sta=i_desc.is_static;

    this->bodystate_array.add_body(&i_desc,&b,1);

    return b;
}

void physicssystem::release_body(body_t* i_body)
{
    killed.push_back(i_body);
}

void physicssystem::create_bodies(body_t* o_body_array[], const bodydesc i_desc[], unsigned i_bodynum)
{
    for (unsigned n=0; n<i_bodynum;++n)
        o_body_array[n]=new (this->body_list.allocate_place()) body_t;

    this->bodystate_array.add_body(i_desc,o_body_array,i_bodynum);
}

void physicssystem::release_bodies(body_t* i_body_array[], unsigned i_bodynum)
{
    for (uint32 n=0; n<i_bodynum; ++n)
        this->killed.push_back(i_body_array[n]);
}

unsigned g_bph=0,g_nph=0,g_in=0,g_up=0,g_frc=0;

void physicssystem::simulate(float i_dt)
{
	if (g_frc==1000)
	{
		g_frc=0;
		g_bph=0;
		g_nph=0;
		g_in=0;
		g_up=0;
	}

	timer_t t;
	++this->frame_count;
    kill_deads();


	t.reset();
    broadphase();
	t.stop();
	g_bph+=t.get_tick();
	t.reset();
    near_phase();
	t.stop();
	g_nph+=t.get_tick();
    update_contacts();
    create_contact_groups();
	t.reset();
    update_inertia();
	t.stop();
	g_in+=t.get_tick();
	t.reset();
    update_bodies(i_dt);
	t.stop();
	g_up+=t.get_tick();
	++g_frc;
}


void kill_deads()
{
    physicssystem* sys=physicssystem::ptr;

    if (sys->killed.size())
    {
        sys->bodystate_array.release_body(&*sys->killed.begin(),sys->killed.size());
        for (uint32 n=0; n<sys->killed.size(); ++n)
		{
            sys->body_list.deallocate_place(sys->killed[n]);
			sys->killed[n]->~body_t();
		}
    }
}


struct update_bounding
{

    void operator()(unsigned i_start,unsigned i_num) const
    {
		const nbody_t& nb=physicssystem::ptr->bodystate_array;
		++i_start;
        uint32 end=i_start+i_num;
        for (uint32 n=i_start; n<end; ++n)
        {
            shape_t* s;//,*endit;
//            endit=nb.body[n]->shapes.last();
            for (s=nb.body[n]->shapes.first(); s; s=s->next)
            {
				s->collider->bounding_world=transform(nb.pos[n],s->bounding);
            }
        }
    }
};


void broadphase()
{
    physicssystem* ptr=physicssystem::ptr;
    const nbody_t& nb=ptr->bodystate_array;

    if (ptr->parallel_boudingupdate)
    {
        taskmanager::ptr->process_buffer(nb.size-1,10,update_bounding());
    }
    else
    {
        update_bounding ub;
        ub(0,nb.size-1);
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

            if (shape1->body!=shape2->body && ((shape1->owner_flag & shape2->collision_mask) || (shape2->owner_flag & shape1->collision_mask)))
            {
                if (ptr->intersect_fn[shape1->type][shape2->type])
                {
                    vec3 contact[20][2];
                    vec3 normal;
                    unsigned contact_count;
                    const mtx4x3& body1_mtx=ptr->bodystate_array.pos[shape1->body->array_index];
                    const mtx4x3& body2_mtx=ptr->bodystate_array.pos[shape2->body->array_index];
                    int intersect=ptr->intersect_fn[shape1->type][shape2->type](shape1,body1_mtx,
																				shape2,body2_mtx,
																				contact,
																				normal,
																				contact_count);

                    if (intersect)
                    {
                        contact_surface_t* c=ptr->contact_manager.get_contact(shape1->body,shape2->body);

						c->friction=shape1->friction*shape2->friction;
						c->restitution=shape1->restitution*shape2->restitution;
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

    if (ptr->parallel_nearphase)
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
    update_process(nbody_t* i_b,float i_dt,vec3 i_gravity):
	b(i_b),
	dt(i_dt),
	gravity(i_gravity)
	{
	}

	void operator()(unsigned i_start,unsigned i_num) const
	{
		++i_start;
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

	nbody_t* b;
	float dt;
	vec3 gravity;
};

void update_bodies(float i_dt)
{
    physicssystem* ptr=physicssystem::ptr;
    nbody_t* b=&ptr->bodystate_array;
    if (ptr->parallel_update)
    {
        taskmanager::ptr->process_buffer(b->size-1,10,update_process(b,i_dt,ptr->desc.gravity));
    }
    else
    {
        update_process p(b,i_dt,ptr->desc.gravity);
        p(0,b->size-1);
    }
}

struct inertia_process
{
    inertia_process(nbody_t* i_b):
    b(i_b)
    {
    }

    void operator()(unsigned i_start,unsigned i_num) const
    {
		++i_start;
        uint32 end=i_start+i_num;
        for (uint32 n=i_start; n<end; ++n)
        {
            mtx3x3 invpos; invpos.transpose3x3(b->pos[n]);
            mtx3x3 tmp; tmp.multiply3x3(invpos,b->invinertia_rel[n]);
            b->invinertia_abs[n].multiply3x3(tmp,b->pos[n]);
        }
    }

    nbody_t* b;
};

void update_inertia()
{
    physicssystem* ptr=physicssystem::ptr;
    nbody_t* b=&ptr->bodystate_array;

    if (ptr->parallel_inertia)
    {
        taskmanager::ptr->process_buffer(b->size-1,10,inertia_process(b));
    }
    else
    {
        inertia_process i(b);
        i(0,b->size-1);
    }
}


void create_contact_groups()
{
    physicssystem* ptr=physicssystem::ptr;
    ptr->contact_group_manager.create_contact_groups(ptr->bodystate_array.body,
                                                    ptr->bodystate_array.size,
                                                    ptr->contact_manager.contact_list);
}

void update_contacts()
{
    physicssystem* ptr=physicssystem::ptr;
    ptr->contact_manager.update_contacts();
}
