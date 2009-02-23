#include "lcpsolver.h"
#include "threading/taskmanager.h"


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


//#define lcp_data_size (sizeof(jacobi)+sizeof(jacobi)+sizeof(body_index_t)+6*sizeof(float))

void lcp_solver_t::allocate_buffer()
{
	//maximum 4 kontaktpont/kontakt, 1 contactconstraint+1 frictionconstraint/kontaktpont

	lcp_data.J=new jacobi[8*contact_count];
	lcp_data.B=new jacobi[8*contact_count];
	lcp_data.body_index=new body_index_t[8*contact_count];
	lcp_data.right_side=new float[8*contact_count];
	lcp_data.right_side_poscorr=new float[8*contact_count];
	lcp_data.diag=new float[8*contact_count];
	lcp_data.lambda=new float[8*contact_count];
	lcp_data.lambda_poscorr=new float[8*contact_count];
	lcp_data.friction_coeff=new float[8*contact_count];
}

void lcp_solver_t::set_solver_index(body_t* body[2])
{
	if (body[0]->array_index && body[0]->solver_stamp!=physicssystem::ptr->frame_count)
	{
		body[0]->solver_stamp=physicssystem::ptr->frame_count;
		body[0]->solver_index=++body_count;
	}
	if (body[1]->array_index && body[1]->solver_stamp!=physicssystem::ptr->frame_count)
	{
		body[1]->solver_stamp=physicssystem::ptr->frame_count;
		body[1]->solver_index=++body_count;
	}
}

struct solver_pre_step_contacts
{
	lcp_solver_t* solver;
	solver_pre_step_contacts(lcp_solver_t* i_solver): solver(i_solver){}

	void operator()(unsigned start, unsigned count)
	{
		int data_index=8*start;
		lcp_data_t* lcp_data=&solver->lcp_data;
		for (unsigned n=start; n<start+count; ++n)
		{
			contact_surface_t* act_contact=solver->contact_array+n;
			solver->set_solver_index(act_contact->body);

			body_t* body1(act_contact->body[0]);
			body_t* body2(act_contact->body[1]);

			nbody_t* nbody=&physicssystem::ptr->bodystate_array;

			int state_index1=body1->array_index;
			int state_index2=body2->array_index;

			const int solver_index1=body1->solver_index;
			const int solver_index2=body2->solver_index;

			vec3 d;

			for (int k=0; k<act_contact->contact_count; ++k,++data_index)
			{
				contact_point_t* act_cp=act_contact->contactarray+k;
				lcp_data->lambda[data_index]=act_cp->cached_lambda;
				lcp_data->lambda_poscorr[data_index]=0;
				lcp_data->body_index[data_index].i[0]=solver_index1;
				lcp_data->body_index[data_index].i[1]=solver_index2;

				vec3 relpos1=act_cp->abs_pos[0]-nbody->pos[state_index1].t;
				vec3 relpos2=act_cp->abs_pos[1]-nbody->pos[state_index2].t;


				lcp_data->J[data_index].v1=-act_contact->normal;
				lcp_data->J[data_index].w1.cross(act_contact->normal,relpos1);
				lcp_data->J[data_index].v2=act_contact->normal;
				lcp_data->J[data_index].w2.cross(relpos2,act_contact->normal);

				lcp_data->B[data_index].v1=nbody->invmass[state_index1]*lcp_data->J[data_index].v1;
				lcp_data->B[data_index].w1=nbody->invinertia_abs[state_index1].transform3x3(lcp_data->J[data_index].w1);
				lcp_data->B[data_index].v2=nbody->invmass[state_index2]*lcp_data->J[data_index].v2;
				lcp_data->B[data_index].w2=nbody->invinertia_abs[state_index2].transform3x3(lcp_data->J[data_index].w2);

//				vec3 dvel=
			}
		}
	}
};

void lcp_solver_t::pre_step_contacts()
{
	body_count=0;
	physicssystem* ptr=physicssystem::ptr;
	if (ptr->parallel_processing)
	{
		taskmanager::ptr->process_buffer(contact_count,10,solver_pre_step_contacts(this));
	}
	else
	{
		solver_pre_step_contacts spc(this);
		spc(0,contact_count);
	}

}

void lcp_solver_t::solve_contacts()
{
    physicssystem* ptr=physicssystem::ptr;
    for (int contact_index=0; contact_index<contact_count; ++contact_index)
    {
        contact_surface_t* actcontact=contact_array+contact_index;

        int body[2]={actcontact->body[0]->array_index,actcontact->body[1]->array_index};
        for (int n=0; n<actcontact->contact_count; ++n)
        {
            ptr->bodystate_array.vel[body[0]];
            ptr->bodystate_array.vel[body[1]];
            //vec3 body[0]->get_vel()
        }
    }
}

void lcp_solver_t::process(contact_surface_t* i_contact_array, int i_contact_count, float i_dt)
{
	contact_array=i_contact_array;
	contact_count=i_contact_count;
	dt=i_dt;


	allocate_buffer();
	pre_step_contacts();
	solve_contacts();
	clean();
}

