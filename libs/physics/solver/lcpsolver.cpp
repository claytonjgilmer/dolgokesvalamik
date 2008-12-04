#include "lcpsolver.h"


/*
    Jcontact= -n -(r1 x n) n (r2 x n)  0<=lc<inf
    Jfriction= -u -(r1 x u) u (r2 x u)  -lc<=lf<=lc

    MA=JTlambda+Fext
    JV=eps

    A=(V2-V1)/dt
    M(V2-V1)=dt*(JTlambda+Fext) /*M-1
    (V2-V1)=M-1*dt*(JTlambda+Fext) /*J
    JV2-JV1=dt*JM-1JTlambda+dt*M-1*Fext
    JM-1JTlambda=1/dt*(-JV1+eps)-Aext
    JM-1JTy=eps-JV1-VDext

*/

struct jacobi
{
    vec3 v1;
    vec3 w1;
    vec3 v2;
    vec3 w2;
};

struct body_index_t
{
    short i[2];
};


struct lcp_data
{
    jacobi* J;
    jacobi* B;
    body_index_t*
            body_index;
    float*  right_side;
    float*  right_side_poscorr;
    float*  diag;
    float*  lambda;
    float*  lambda_poscorr;
    float*  friction_coeff;


};

#define lcp_data_size (sizeof(jacobi)+sizeof(jacobi)+sizeof(body_index_t)+6*sizeof(float))

void* allocate_buffer(int contact_count)
{
    //maximum 4 kontaktpont/kontakt, 1 contactconstraint+1 frictionconstraint/kontaktpont

    int buf_size=contact_count*(8*lcp_data_size);
    return malloc(buf_size);
}

void pre_step(lcp_solver_t* solver, contact_t* contact_array, int contact_count, float dt)
{
}

void solve(lcp_solver_t* solver, contact_t* contact_array[], int contact_count)
{
    physicssystem* ptr=physicssystem::ptr;
    for (int contact_index=0; contact_index<contact_count; ++contact_index)
    {
        contact_t* actcontact=contact_array[contact_index];

        int body[2]={actcontact->shape[0]->body->array_index,actcontact->shape[1]->body->array_index};
        int is_static[2]={actcontact->shape[0]->body->is_static,actcontact->shape[1]->body->is_static};
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

