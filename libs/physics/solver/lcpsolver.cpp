#include "lcpsolver.h"

void pre_step(lcp_solver_t* solver, contact_t* contact_array, int contact_count, float dt)
{
}

void solve(lcp_solver_t* solver, contact_t* contact_array[], int contact_count)
{
    physicssystem* ptr=physicssystem::ptr;
    for (int contact_index=0; contact_index<contact_count; ++contact_index)
    {
        contact_t* actcontact=contact_array[contact_index];

        int body[2]={actcontact->body[0]->array_index,actcontact->body[1]->array_index};
        int is_static[2]={actcontact->body[0]->is_static,actcontact->body[1]->is_static};
        for (int n=0; n<actcontact->contact_count; ++n)
        {
            ptr->bodystate_array[is_static[0]].vel[body[0]];
            ptr->bodystate_array[is_static[1]].vel[body[1]];
            //vec3 body[0]->get_vel()
        }
    }
}

void lcp_solver_t::process(contact_t* contact_array, int contact_count, float dt)
{
    pre_step(this,contact_array,contact_count, dt);
//    solve(this);
}

