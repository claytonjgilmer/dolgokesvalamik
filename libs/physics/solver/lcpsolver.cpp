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
	ZeroMemory(&lcp_data_contact,sizeof(lcp_data_t));
	ZeroMemory(&lcp_data_friction,sizeof(lcp_data_t));

	lcp_data_contact.J=new jacobi[4*contact_count];
	lcp_data_contact.B=new jacobi[4*contact_count];
	lcp_data_contact.body_index=new body_index_t[4*contact_count];
	lcp_data_contact.right_side=new float[4*contact_count];
	lcp_data_contact.right_side_poscorr=new float[4*contact_count];
	lcp_data_contact.diag=new float[4*contact_count];
	lcp_data_contact.lambda=new float[4*contact_count];
	lcp_data_contact.lambda_poscorr=new float[4*contact_count];
//	lcp_data_contact.friction_coeff=new float[4*contact_count];
	lcp_data_contact.constraintnum=new char[contact_count];

	lcp_data_friction.J=new jacobi[4*contact_count];
	lcp_data_friction.B=new jacobi[4*contact_count];
//	lcp_data_friction.body_index=new body_index_t[8*contact_count];
	lcp_data_friction.right_side=new float[4*contact_count];
//	lcp_data_friction.right_side_poscorr=new float[8*contact_count];
	lcp_data_friction.diag=new float[4*contact_count];
	lcp_data_friction.lambda=new float[4*contact_count];
//	lcp_data_contact.lambda_poscorr=new float[4*contact_count];
	lcp_data_friction.friction_coeff=new float[4*contact_count];
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
		int constraint_index=4*start;
		physicssystem* ptr=physicssystem::ptr;
		float pos_corr_rate=ptr->solver_position_correction_rate;
		lcp_data_t* lcp_data=&solver->lcp_data_contact;
		lcp_data_t* lcp_data_friction=&solver->lcp_data_friction;

		for (unsigned n=start; n<start+count; ++n)
		{
			contact_surface_t* act_contact=solver->contact_array+n;

			solver->set_solver_index(act_contact->body);

			const int solver_index1=act_contact->body[0]->solver_index;
			const int solver_index2=act_contact->body[1]->solver_index;

			int body1=act_contact->body[0]->array_index;
			int body2=act_contact->body[1]->array_index;

			nbody_t* nbody=&physicssystem::ptr->bodystate_array;
			int constraint_index_save=constraint_index;

			for (int k=0; k<act_contact->contact_count; ++k,++constraint_index)
			{
				contact_point_t* act_cp=act_contact->contactarray+k;
				lcp_data->lambda[constraint_index]=act_cp->cached_lambda;
				lcp_data->lambda_poscorr[constraint_index]=0;
				lcp_data->body_index[constraint_index].i[0]=solver_index1;
				lcp_data->body_index[constraint_index].i[1]=solver_index2;

				vec3 relpos1=act_cp->abs_pos[0]-nbody->pos[body1].t;
				vec3 relpos2=act_cp->abs_pos[1]-nbody->pos[body2].t;

				jacobi* j=lcp_data->J+constraint_index;
				jacobi* b=lcp_data->B+constraint_index;


				j->v1=-act_contact->normal;
				j->w1.cross(act_contact->normal,relpos1);
				j->v2=act_contact->normal;
				j->w2.cross(relpos2,act_contact->normal);

				b->v1=nbody->invmass[body1]*j->v1;
				b->w1=nbody->invinertia_abs[body1].transform3x3(j->w1);
				b->v2=nbody->invmass[body2]*j->v2;
				b->w2=nbody->invinertia_abs[body2].transform3x3(j->w2);

				vec3 dv=	nbody->vel[body2]+
					cross(nbody->rotvel[body2], relpos2) -
					nbody->vel[body1]-
					cross(nbody->rotvel[body1], relpos1);

				float vel_normal = dot(dv, act_contact->normal);
				float biasVelocityPositionCorrection=pos_corr_rate*act_cp->penetration/solver->dt;
				float restitution=-min(vel_normal*act_contact->restitution+0.05f,0.0f);

				lcp_data->right_side[constraint_index]=restitution-//(1+contactptr->m_Restitution)*
					(dot(j->v1,nbody->vel[body1])+
					dot(j->w1,nbody->rotvel[body1])+
					dot(j->v2,nbody->vel[body2])+
					dot(j->w2,nbody->rotvel[body2]));// /i_DeltaTime;

				lcp_data->right_side_poscorr[constraint_index]=biasVelocityPositionCorrection;

				lcp_data->diag[constraint_index]=1/(dot(j->v1,b->v1)+
					dot(j->w1,b->w1)+
					dot(j->v2,b->v2)+
					dot(j->w2,b->w2));


				//friction
				lcp_data_friction->lambda[constraint_index]=0;

				float maxVal=abs(act_contact->normal.x);
				int maxIndex=0;

				if (maxVal<abs(act_contact->normal.y))
				{
					maxVal=abs(act_contact->normal.y);
					maxIndex=1;
				}

				if (maxVal<abs(act_contact->normal.z))
					maxIndex=2;

				const int otherIndex=nextnumbermodulo3(maxIndex);
				const int thirdIndex=nextnumbermodulo3(otherIndex);

				vec3 tangentDirection1=act_contact->normal;

				swap(tangentDirection1[maxIndex],tangentDirection1[otherIndex]);
				tangentDirection1[maxIndex]=-tangentDirection1[maxIndex];
				tangentDirection1[thirdIndex]=0;


				tangentDirection1.normalize();

				vec3 tangentDirection2; tangentDirection2.cross(tangentDirection1,act_contact->normal);

				vec3 tangentDirection=	tangentDirection1*dot(dv,tangentDirection1)+
					tangentDirection2*dot(dv,tangentDirection2);

				if (tangentDirection.squarelength()>0.000001f)
					tangentDirection.normalize();
				else
					tangentDirection=tangentDirection1;

				jacobi* fj=lcp_data_friction->J+constraint_index;
				jacobi* fb=lcp_data_friction->B+constraint_index;

				fj->v1=-tangentDirection;
				fj->w1.cross(tangentDirection,relpos1);
				fj->v2=tangentDirection;
				fj->w2.cross(relpos2,tangentDirection);

				fb->v1=nbody->invmass[body1]*fj->v1;
				nbody->invinertia_abs[body1].transform3x3(fb->w1,fj->w1);
				fb->v2=nbody->invmass[body2]*fj->v2;
				nbody->invinertia_abs[body2].transform3x3(fb->w2,fj->w2);

				lcp_data_friction->right_side[constraint_index]=-
					(dot(fj->v1,nbody->vel[body1])+
					dot(fj->w1,nbody->rotvel[body1])+
					dot(fj->v2,nbody->vel[body2])+
					dot(fj->w2,nbody->rotvel[body2]));// /i_DeltaTime;



				lcp_data_friction->diag[constraint_index]=1.0f/
					(dot(fj->v1,fb->v1)+
					dot(fj->w1,fb->w1)+
					dot(fj->v2,fb->v2)+
					dot(fj->w2,fb->w2));
			}

			constraint_index=constraint_index_save+4;
		}
	}
};

void lcp_solver_t::pre_step_contacts_and_frictions()
{
	body_count=0;
	physicssystem* ptr=physicssystem::ptr;
	if (ptr->parallel_nearphase)
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
	int constraint_index=0;
	for (int actcontact=0; actcontact<contact_count; ++actcontact)
	{
		const int body1=lcp_data_contact.body_index[constraint_index].i[0];
		const int body2=lcp_data_contact.body_index[constraint_index].i[0];

		const int numconstraint=lcp_data_contact.constraintnum[actcontact];

		const jacobi* __restrict jPtr=lcp_data_contact.J+constraint_index;
		const jacobi* __restrict bPtr=lcp_data_contact.B+constraint_index;
		accel_t* __restrict accPtr=accel;

		int constraint_index_save=constraint_index;
		for (int actconstraint=0; actconstraint<numconstraint; ++actconstraint,++constraint_index)
		{
			float newLambda=lcp_data_contact.right_side[constraint_index]-
				dot(jPtr[actconstraint].v1,accPtr[body1].v)-
				dot(jPtr[actconstraint].w1,accPtr[body1].w)-
				dot(jPtr[actconstraint].v2,accPtr[body2].v)-
				dot(jPtr[actconstraint].w2,accPtr[body2].w);

			newLambda*= lcp_data_contact.diag[constraint_index];;
			newLambda+=lcp_data_contact.lambda[constraint_index];

			if (newLambda<0)
				newLambda=0;

			float deltaLambda=newLambda-lcp_data_contact.lambda[constraint_index];
			lcp_data_contact.lambda[constraint_index]=newLambda;

			accPtr[body1].v+=deltaLambda* bPtr[actconstraint].v1;
			accPtr[body1].w+=deltaLambda* bPtr[actconstraint].w1;

			accPtr[body2].v+=deltaLambda* bPtr[actconstraint].v2;
			accPtr[body2].w+=deltaLambda* bPtr[actconstraint].w2;

			newLambda=lcp_data_contact.right_side_poscorr[constraint_index]-
				dot(jPtr[actconstraint].v1,accPtr[body1].p)-
				dot(jPtr[actconstraint].w1,accPtr[body1].o)-
				dot(jPtr[actconstraint].v2,accPtr[body2].p)-
				dot(jPtr[actconstraint].w2,accPtr[body2].o);

			newLambda*= lcp_data_contact.diag[constraint_index];
			newLambda+=lcp_data_contact.lambda_poscorr[constraint_index];

			if (newLambda<0)
				newLambda=0;

			deltaLambda=newLambda-lcp_data_contact.lambda_poscorr[actconstraint];

			lcp_data_contact.lambda_poscorr[actconstraint]=newLambda;

			accPtr[body1].p+=deltaLambda*bPtr[actconstraint].v1;
			accPtr[body1].o+=deltaLambda*bPtr[actconstraint].w1;

			accPtr[body2].p+=deltaLambda*bPtr[actconstraint].v2;
			accPtr[body2].o+=deltaLambda*bPtr[actconstraint].w2;
		}

		constraint_index=constraint_index_save+4;
	}
}

void lcp_solver_t::solve_constraints()
{
	for (int it=0; it<10; ++it)
	{
		solve_contacts();
		solve_frictions();
	}
}

void lcp_solver_t::process(contact_surface_t* i_contact_array, int i_contact_count, float i_dt)
{
	contact_array=i_contact_array;
	contact_count=i_contact_count;
	dt=i_dt;


	allocate_buffer();
	pre_step();
	solve_constraints();
	clean();
}

void lcp_solver_t::pre_step()
{
	pre_step_contacts_and_frictions();
	init_acceleration();
}

struct init_accel_process
{
	init_accel_process(lcp_solver_t* i_solver):solver(i_solver){}

	void operator()(unsigned start, unsigned count)
	{
		int constraint_index=4*start;
		for (unsigned actcontact=start; actcontact<start+count; ++actcontact)
		{
			const int body1=solver->lcp_data_contact.body_index[constraint_index].i[0];
			const int body2=solver->lcp_data_contact.body_index[constraint_index].i[1];
			const int constraint_count=solver->lcp_data_contact.constraintnum[n];

			jacobi* bptr=solver->lcp_data_contact.B[constraint_index];
			int constraint_index_save=constraint_index;
			for (int actconstraint=0; actconstraint<constraint_count; ++actconstraint,++constraint_index)
			{
				const float lambda=solver->lcp_data_contact.lambda[constraint_index];
				const float lambdaposcorr=solver->lcp_data_contact.lambda_poscorr[constraint_index];

				solver->accel[body1].v+=lambda*bptr[actconstraint].m_V1;
				solver->accel[body1].m_W+=lambda*bptr[actconstraint].m_W1;

				solver->accel[body2].m_V+=lambda*bptr[actconstraint].m_V2;
				solver->accel[body2].m_W+=lambda*bptr[actconstraint].m_W2;

				solver->accel[body1].m_P+=lambdaposcorr*bptr[actconstraint].m_V1;
				solver->accel[body1].m_O+=lambdaposcorr*bptr[actconstraint].m_W1;

				solver->accel[body2].m_P+=lambdaposcorr*bptr[actconstraint].m_V2;
				solver->accel[body2].m_O+=lambdaposcorr*bptr[actconstraint].m_W2;
			}

			constraint_index=constraint_index_save+4;
		}
	}

	lcp_solver_t* solver;
};

void lcp_solver_t::init_acceleration()
{
	accel=new accel_t[body_count];

	for (int n=0; n<body_count; ++n)
	{
		accel[n].v.clear();
		accel[n].w.clear();
		accel[n].p.clear();
		accel[n].o.clear();
	}

	if (physicssystem::ptr->parallel_init_accel)
	{
		taskmanager::ptr->process_buffer(contact_count,10,init_accel_process(this));
	}
	else
	{
		init_accel_process s(this); s(0,contact_count);
	}

	for (int actcontact=0; actcontact<contact_count; actcontact++)
	{
		const int indexBody1=m_ContactBodyIndex[actcontact][0];
		const int indexBody2=m_ContactBodyIndex[actcontact][1];

		const int numconstraints=m_ContactConstraintNum[actcontact];

		JacobiStruct* bptr=m_ContactBMatrix[actcontact];
		for (int actconstraint=0; actconstraint<numconstraints; ++actconstraint)
		{
			const float lambda=m_LambdaContact[actcontact][actconstraint];
			const float lambdaposcorr=m_LambdaContactPosCorr[actcontact][actconstraint];

			m_Acceleration[indexBody1].m_V+=lambda*bptr[actconstraint].m_V1;
			m_Acceleration[indexBody1].m_W+=lambda*bptr[actconstraint].m_W1;

			m_Acceleration[indexBody2].m_V+=lambda*bptr[actconstraint].m_V2;
			m_Acceleration[indexBody2].m_W+=lambda*bptr[actconstraint].m_W2;

			m_Acceleration[indexBody1].m_P+=lambdaposcorr*bptr[actconstraint].m_V1;
			m_Acceleration[indexBody1].m_O+=lambdaposcorr*bptr[actconstraint].m_W1;

			m_Acceleration[indexBody2].m_P+=lambdaposcorr*bptr[actconstraint].m_V2;
			m_Acceleration[indexBody2].m_O+=lambdaposcorr*bptr[actconstraint].m_W2;
		}
	}


}


