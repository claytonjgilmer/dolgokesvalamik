#include "physicssystem.h"
#include "threading/taskmanager2.h"
#include "utils/performancemeter.h"
#include "physics/solver/lcpsolver.h"
#include "physics/collision/shapes/convexmeshshape.h"

#define taskmanager taskmanager2_t

DEFINE_SINGLETON(physicssystem);

void kill_deads();
void update_inertia();
void update_velocities(f32 dt);
void update_positions(f32 dt);
//void update_bodies(f32 dt);
void broadphase();
void near_phase();
void create_contact_groups();
void update_contacts();
void solve_constraints(f32 dt);
void draw_contacts();

body_t* g_world=NULL;


void create_world_body(physicssystem* ptr)
{
	g_world=new (ptr->body_list.allocate_place()) body_t;
#ifdef _DEBUG
	g_world->set_name("WORLD");
#endif
	ptr->bodystate_array.add_world();
}

physicssystem::physicssystem(const physicssystemdesc* i_desc):
desc(*i_desc)
{
    ZeroMemory(this->intersect_fn,sizeof(this->intersect_fn));
    this->intersect_fn[shape_type_sphere][shape_type_sphere]=&test_sphere_sphere_intersect;
	this->intersect_fn[shape_type_box][shape_type_sphere]=&test_box_sphere_intersect;
	this->intersect_fn[shape_type_sphere][shape_type_box]=&test_sphere_box_intersect;
	this->intersect_fn[shape_type_convex_mesh][shape_type_convex_mesh]=&test_convex_convex_intersect<convex_mesh_shape_t,convex_mesh_shape_t>;
	this->intersect_fn[shape_type_convex_mesh][shape_type_sphere]=&test_convex_convex_intersect<convex_mesh_shape_t,sphere_shape>;
	this->intersect_fn[shape_type_sphere][shape_type_convex_mesh]=&test_convex_convex_intersect<sphere_shape,convex_mesh_shape_t>;
	this->intersect_fn[shape_type_convex_mesh][shape_type_box]=&test_convex_convex_intersect<convex_mesh_shape_t,box_shape>;
	this->intersect_fn[shape_type_box][shape_type_convex_mesh]=&test_convex_convex_intersect<box_shape,convex_mesh_shape_t>;
    this->parallel_boudingupdate=
		parallel_broadphase=
		parallel_nearphase=
		parallel_update=
		parallel_inertia=
		parallel_pre_step=
		parallel_solver=
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

//unsigned g_bph=0,g_nph=0,g_in=0,g_up=0,g_frc=0,g_sol=0,g_presol=0,g_solcon=0,g_solv_cont=0,g_solv_fric=0,g_cache_l=0;

void physicssystem::simulate(f32 i_dt)
{
	perf_meter(perf_simulate);
	++this->frame_count;
    kill_deads();

	{
		perf_meter(perf_broadphase);
		broadphase();
	}

	{
		perf_meter(perf_nearphase);
	    near_phase();
	}

    update_contacts();
    create_contact_groups();

	draw_contacts();

	{
		perf_meter(perf_inertia);
		update_inertia();
	}

	update_velocities(i_dt);

	solve_constraints(i_dt);

	{
		perf_meter(perf_updateposition);
		update_positions(i_dt);
	}
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

    void operator()(unsigned i_start,unsigned i_num)
    {
		nbody_t& nb=physicssystem::ptr->bodystate_array;
		++i_start;
        uint32 end=i_start+i_num;
        for (uint32 n=i_start; n<end; ++n)
        {
            shape_t* s;//,*endit;
//            endit=nb.body[n]->shapes.last();
            for (s=nb.get_body(n)->shapes.first(); s; s=s->next)
            {
				s->collider->bounding_world=transform(nb.get_pos(n),s->bounding);
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
        taskmanager2_t::ptr->process_buffer(nb.get_size()-1,10,update_bounding());
    }
    else
    {
        update_bounding ub;
        ub(0,nb.get_size()-1);
    }

    ptr->broad_phase.update();
}

struct near_struct
{
	contact_surface_t** surface_array;
	near_struct(contact_surface_t* surface_array[]): surface_array(surface_array){}
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
                    const mtx4x3& body1_mtx=ptr->bodystate_array.get_pos(shape1->body->array_index);
                    const mtx4x3& body2_mtx=ptr->bodystate_array.get_pos(shape2->body->array_index);
                    int intersect=ptr->intersect_fn[shape1->type][shape2->type](shape1,body1_mtx,
																				shape2,body2_mtx,
																				contact,
																				normal,
																				contact_count);

                    if (intersect)
                    {
                        contact_surface_t* c=surface_array[n];// ptr->contact_manager.get_contact(shape1->body,shape2->body);

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

void collect_contact_surfaces(contact_surface_t* surface_array[])
{
	physicssystem* ptr=physicssystem::ptr;
	const broadphasepair* array=ptr->broad_phase.pair_array;
	for (int n=0; n<ptr->broad_phase.pair_num; ++n)
	{
		shape_t* shape1=(shape_t*)array[n].object[0]->userdata;
		shape_t* shape2=(shape_t*)array[n].object[1]->userdata;
		surface_array[n]=ptr->contact_manager.get_contact(shape1->body,shape2->body);
	}
}

void near_phase()
{
    physicssystem* ptr=physicssystem::ptr;

	contact_surface_t* surface_array[65536];
	collect_contact_surfaces(surface_array);

    if (ptr->parallel_nearphase)
    {
		if (ptr->broad_phase.pair_num)
			taskmanager::ptr->process_buffer(ptr->broad_phase.pair_num,10,near_struct(surface_array));
    }
    else
    {
        near_struct ns(surface_array);
        ns(0,ptr->broad_phase.pair_num);
    }
}

struct update_vel_process
{
    update_vel_process(nbody_t* i_b,f32 i_dt,vec3 i_gravity):
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
			b->get_vel(n)+=dt*(b->get_invmass(n)*b->get_force(n)+gravity);
			b->get_rotvel(n)+=dt*b->get_invinertia_abs(n).transform3x3(b->get_torque(n));

			b->get_force(n).clear();
			b->get_torque(n).clear();
		}
	}

	nbody_t* b;
	f32 dt;
	vec3 gravity;
};

struct update_pos_process
{
	update_pos_process(nbody_t* i_b,f32 i_dt):
b(i_b),
dt(i_dt)
{
}

void operator()(unsigned i_start,unsigned i_num) const
{
	++i_start;
	uint32 end=i_start+i_num;
	for (uint32 n=i_start; n<end; ++n)
	{
		b->get_vel(n)+=b->get_constraint_accel(n).v;
		b->get_rotvel(n)+=b->get_constraint_accel(n).w;
		b->get_pos(n).t+=dt*(b->get_vel(n)+b->get_constraint_accel(n).p);
		vec3 axis=b->get_rotvel(n)+b->get_constraint_accel(n).o;
		f32 angle=axis.length();

		if (angle>0.001f)
			b->get_pos(n).rotate(axis/angle,dt*angle);

		b->get_constraint_accel(n).clear();
	}
}

nbody_t* b;
f32 dt;
};

void update_positions(f32 i_dt)
{
    physicssystem* ptr=physicssystem::ptr;
    nbody_t* b=&ptr->bodystate_array;
    if (ptr->parallel_update)
    {
        taskmanager::ptr->process_buffer(b->get_size()-1,10,update_pos_process(b,i_dt));
    }
    else
    {
        update_pos_process p(b,i_dt);
        p(0,b->get_size()-1);
    }
}

void update_velocities(f32 i_dt)
{
	physicssystem* ptr=physicssystem::ptr;
	nbody_t* b=&ptr->bodystate_array;
	if (ptr->parallel_update)
	{
		taskmanager::ptr->process_buffer(b->get_size()-1,10,update_vel_process(b,i_dt,ptr->desc.gravity));
	}
	else
	{
		update_vel_process p(b,i_dt,ptr->desc.gravity);
		p(0,b->get_size()-1);
	}
}

struct constraint_solver_t
{
	f32 dt;
	constraint_solver_t(f32 i_dt):dt(i_dt){}

	void operator()(unsigned start, unsigned num) const
	{
		physicssystem* ptr=physicssystem::ptr;
		contact_group_manager_t* cgm=&ptr->contact_group_manager;
		contact_surface_t** contact_array=&(cgm->contact_array[0]);
		for (unsigned n=start; n<start+num; ++n)
		{
			lcp_solver_t solver;
			solver.process(contact_array+cgm->group_array[n].first_contact,cgm->group_array[n].contact_count,dt);
		}
	}
};


void solve_constraints(f32 dt)
{
	perf_meter(perf_solve);
	physicssystem* ptr=physicssystem::ptr;

	if (ptr->contact_group_manager.group_array.size()==0)
		return;

	if (ptr->contact_group_manager.group_array.size()==1)
		return;
	if (ptr->parallel_solver)
	{
		taskmanager::ptr->process_buffer(ptr->contact_group_manager.group_array.size()-1,1,constraint_solver_t(dt));
	}
	else
	{
		constraint_solver_t cs(dt); cs(0,ptr->contact_group_manager.group_array.size()-1);
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
            mtx3x3 invpos; invpos.transpose3x3(b->get_pos(n));
//            mtx3x3 tmp; tmp.multiply3x3(invpos,b->invinertia_rel[n]);
			mtx3x3 tmp; tmp.multiply3x3(&invpos,&b->get_invinertia_rel(n));
//            b->invinertia_abs[n].multiply3x3(tmp,b->pos[n]);
			b->get_invinertia_abs(n).multiply3x3(&tmp,&b->get_pos(n));
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
        taskmanager::ptr->process_buffer(b->get_size()-1,10,inertia_process(b));
    }
    else
    {
        inertia_process i(b);
        i(0,b->get_size()-1);
    }
}


void create_contact_groups()
{
    physicssystem* ptr=physicssystem::ptr;
    ptr->contact_group_manager.create_contact_groups(ptr->bodystate_array,ptr->contact_manager.contact_list);
}

void update_contacts()
{
    physicssystem* ptr=physicssystem::ptr;
    ptr->contact_manager.update_contacts();
}

void draw_contacts()
{
	physicssystem::ptr->contact_manager.draw_contacts();
}
