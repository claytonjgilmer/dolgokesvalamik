#include "physicssystem.h"
#include "threading/taskmanager2.h"

DEFINE_SINGLETON(physicssystem);

void kill_deads();
void update_inertia();
void update_bodies(float dt);
void broadphase();
void near_phase();


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
    update_inertia();
    update_bodies(i_dt);
}


void kill_deads()
{
    physicssystem* sys=physicssystem::ptr();

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
		const nbody& nb=physicssystem::ptr()->bodystate_array[BODYSTATE_DYNAMIC];
        uint32 end=i_start+i_num;
        for (uint32 n=i_start; n<end; ++n)
        {
            intr_list<shape_t>::iterator it,endit;
            endit=nb.body[n]->shapes.end();
            for (it=nb.body[n]->shapes.begin(); it!=endit; ++it)
            {
                shape_t* s=(*it);
				s->collider->bounding_world=transform(nb.pos[n],s->bounding);
            }
//			for (shape_t* s=nb.body[n]->shapes;s;s=s->next)
//				s->collider->bounding_world=transform(nb.pos[n],s->bounding);
        }
    }
};


void broadphase()
{
    physicssystem* ptr=physicssystem::ptr();
    const nbody& nb=ptr->bodystate_array[BODYSTATE_DYNAMIC];

    if (ptr->parallel_processing)
    {
        taskmanager::ptr()->process_buffer(nb.size,10,update_bounding());
    }
    else
    {
        for (unsigned n=0; n<nb.size; ++n)
        {
            body_t* b=nb.body[n];

            intr_list<shape_t>::iterator it,endit;
            endit=b->shapes.end();
            for (it=b->shapes.begin(); it!=endit; ++it)
            {
                shape_t* s=(*it);
                s->collider->bounding_world=transform(nb.pos[n],s->bounding);
            }
        }
    }

    ptr->broad_phase.update();
}

struct near_struct
{
    void operator()(unsigned i_start,unsigned i_num) const
    {
        physicssystem* const ptr=physicssystem::ptr();
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
                    unsigned contactnum;
                    int intersect=ptr->intersect_fn[shape1->type][shape2->type](shape1,ptr->bodystate_array[shape1->body->is_static].pos[shape1->body->array_index],shape2,ptr->bodystate_array[shape2->body->is_static].pos[shape2->body->array_index],contact,normal,contactnum);

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
                        contact_t* c=ptr->cm.get_contact(shape1->body,shape2->body);
                    }
                }
            }
        }
    }
};

void near_phase()
{
    physicssystem* ptr=physicssystem::ptr();

    if (ptr->parallel_processing)
    {
		if (ptr->broad_phase.pair_num)
			taskmanager::ptr()->process_buffer(ptr->broad_phase.pair_num,10,near_struct());
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
    physicssystem* ptr=physicssystem::ptr();
    if (ptr->parallel_processing)
    {
        nbody* b=ptr->bodystate_array+BODYSTATE_DYNAMIC;
        taskmanager::ptr()->process_buffer(b->size,10,update_process(b,i_dt,ptr->desc.gravity));
    }
    else
    {
        nbody* b=ptr->bodystate_array+BODYSTATE_DYNAMIC;
        for (uint32 n=0; n<b->size;++n)
        {
            b->vel[n]+=i_dt*(b->invmass[n]*b->force[n]+ptr->desc.gravity);
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
    physicssystem* ptr=physicssystem::ptr();
    nbody* b=ptr->bodystate_array+BODYSTATE_DYNAMIC;

    if (ptr->parallel_processing)
    {
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
