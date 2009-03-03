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


//#define lcp_data_size (sizeof(jacobi)+sizeof(jacobi)+sizeof(body_index_t)+6*sizeof(f32))

//accel_t lcp_solver_t::accel[65536];

void lcp_solver_t::allocate_buffer()
{
	//maximum 4 kontaktpont/kontakt, 1 contactconstraint+1 frictionconstraint/kontaktpont
//	ZeroMemory(&lcp_data_contact,sizeof(lcp_data_t));
//	ZeroMemory(&lcp_data_friction,sizeof(lcp_data_t));

	lcp_data_contact.J.resize(4*contact_count);
	lcp_data_contact.B.resize(4*contact_count);
	lcp_data_contact.right_side.resize(4*contact_count);
	lcp_data_contact.right_side_poscorr.resize(4*contact_count);
	lcp_data_contact.diag.resize(4*contact_count);
	lcp_data_contact.lambda.resize(4*contact_count);
	lcp_data_contact.lambda_poscorr.resize(4*contact_count);
	lcp_data_contact.constraintnum.resize(contact_count);
	lcp_data_contact.body_index.resize(contact_count);

	lcp_data_friction.J.resize(4*contact_count);
	lcp_data_friction.B.resize(4*contact_count);
	lcp_data_friction.right_side.resize(4*contact_count);
	lcp_data_friction.diag.resize(4*contact_count);
	lcp_data_friction.lambda.resize(4*contact_count);
	lcp_data_friction.friction_coeff.resize(contact_count);
#if 0
	lcp_data_contact.J=new jacobi[4*contact_count];
	lcp_data_contact.B=new jacobi[4*contact_count];
	lcp_data_contact.right_side=new f32[4*contact_count];
	lcp_data_contact.right_side_poscorr=new f32[4*contact_count];
	lcp_data_contact.diag=new f32[4*contact_count];
	lcp_data_contact.lambda=new f32[4*contact_count];
	lcp_data_contact.lambda_poscorr=new f32[4*contact_count];
	lcp_data_contact.constraintnum=new char[contact_count];
	lcp_data_contact.body_index=new body_index_t[contact_count];

	lcp_data_friction.J=new jacobi[4*contact_count];
	lcp_data_friction.B=new jacobi[4*contact_count];
	lcp_data_friction.right_side=new f32[4*contact_count];
	lcp_data_friction.diag=new f32[4*contact_count];
	lcp_data_friction.lambda=new f32[4*contact_count];
	lcp_data_friction.friction_coeff=new f32[contact_count];
#endif
}

struct solver_pre_step_contacts
{
	lcp_solver_t* solver;
	solver_pre_step_contacts(lcp_solver_t* i_solver): solver(i_solver){}

	void operator()(unsigned start, unsigned count)
	{
		int constraint_index=4*start;
		physicssystem* ptr=physicssystem::ptr;
		f32 pos_corr_rate=ptr->solver_position_correction_rate;
		lcp_data_t* lcp_data=&solver->lcp_data_contact;
		lcp_data_t* lcp_data_friction=&solver->lcp_data_friction;

		for (unsigned n=start; n<start+count; ++n)
		{
			contact_surface_t* act_contact=solver->contact_array[n];

			int body1=act_contact->body[0]->array_index;
			int body2=act_contact->body[1]->array_index;
			lcp_data->body_index[n].i[0]=body1;
			lcp_data->body_index[n].i[1]=body2;

			lcp_data->constraintnum[n]=act_contact->contact_count;
			lcp_data_friction->friction_coeff[n]=act_contact->friction;

			nbody_t* nbody=&physicssystem::ptr->bodystate_array;
			int constraint_index_save=constraint_index;

			for (int k=0; k<act_contact->contact_count; ++k,++constraint_index)
			{
				contact_point_t* act_cp=act_contact->contactarray+k;
				lcp_data->lambda[constraint_index]=act_cp->cached_lambda;
				lcp_data->lambda_poscorr[constraint_index]=0;

				vec3 relpos1=act_cp->abs_pos[0]-nbody->pos[body1].t;
				vec3 relpos2=act_cp->abs_pos[1]-nbody->pos[body2].t;

#ifdef _DEBUG
				jacobi* j=&lcp_data->J[constraint_index];
				jacobi* b=&lcp_data->B[constraint_index];
#else
				jacobi* j=lcp_data->J+constraint_index;
				jacobi* b=lcp_data->B+constraint_index;
#endif


				j->v1=-act_contact->normal;
				j->w1.cross(act_contact->normal,relpos1);
				assertion(fabsf(j->w1.x)<10000);
				j->v2=act_contact->normal;
				j->w2.cross(relpos2,act_contact->normal);

				b->v1=nbody->invmass[body1]*j->v1;
				b->w1=nbody->invinertia_abs[body1].transform3x3(j->w1);
				b->v2=nbody->invmass[body2]*j->v2;
				b->w2=nbody->invinertia_abs[body2].transform3x3(j->w2);

				const f32 lambda=solver->lcp_data_contact.lambda[constraint_index];
				nbody->constraint_accel[body1].v+=lambda*b->v1;
				nbody->constraint_accel[body1].w+=lambda*b->w1;

				nbody->constraint_accel[body2].v+=lambda*b->v2;
				nbody->constraint_accel[body2].w+=lambda*b->w2;



				vec3 dv=	nbody->vel[body2]+
					cross(nbody->rotvel[body2], relpos2) -
					nbody->vel[body1]-
					cross(nbody->rotvel[body1], relpos1);

				f32 vel_normal = dot(dv, act_contact->normal);
				f32 biasVelocityPositionCorrection=-pos_corr_rate*act_cp->penetration/solver->dt;
				f32 restitution=-min(vel_normal*act_contact->restitution+0.05f,0.0f);

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

				f32 maxVal=abs(act_contact->normal.x);
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

#ifdef _DEBUG
				jacobi* fj=&lcp_data_friction->J[constraint_index];
				jacobi* fb=&lcp_data_friction->B[constraint_index];
#else
				jacobi* fj=lcp_data_friction->J+constraint_index;
				jacobi* fb=lcp_data_friction->B+constraint_index;
#endif

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
	accel_t* __restrict accPtr=ptr->bodystate_array.constraint_accel;
	for (int actcontact=0; actcontact<contact_count; ++actcontact)
	{
		const int body1=lcp_data_contact.body_index[actcontact].i[0];
		const int body2=lcp_data_contact.body_index[actcontact].i[1];

		const int numconstraint=lcp_data_contact.constraintnum[actcontact];

#ifdef _DEBUG
		const jacobi* __restrict jPtr=&lcp_data_contact.J[constraint_index];
		const jacobi* __restrict bPtr=&lcp_data_contact.B[constraint_index];
#else
		const jacobi* __restrict jPtr=lcp_data_contact.J+constraint_index;
		const jacobi* __restrict bPtr=lcp_data_contact.B+constraint_index;
#endif

		int constraint_index_save=constraint_index;
		for (int actconstraint=0; actconstraint<numconstraint; ++actconstraint,++constraint_index)
		{
			f32 newLambda=lcp_data_contact.right_side[constraint_index]-
				dot(jPtr[actconstraint].v1,accPtr[body1].v)-
				dot(jPtr[actconstraint].w1,accPtr[body1].w)-
				dot(jPtr[actconstraint].v2,accPtr[body2].v)-
				dot(jPtr[actconstraint].w2,accPtr[body2].w);

			newLambda*= lcp_data_contact.diag[constraint_index];;
			newLambda+=lcp_data_contact.lambda[constraint_index];

			if (newLambda<0)
				newLambda=0;

			f32 deltaLambda=newLambda-lcp_data_contact.lambda[constraint_index];
			lcp_data_contact.lambda[constraint_index]=newLambda;

			accPtr[body1].v+=deltaLambda* bPtr[actconstraint].v1;
			accPtr[body1].w+=deltaLambda* bPtr[actconstraint].w1;

			accPtr[body2].v+=deltaLambda* bPtr[actconstraint].v2;
			accPtr[body2].w+=deltaLambda* bPtr[actconstraint].w2;
			assertion(accPtr[body1].w.squarelength()<0.001f);
			assertion(accPtr[body2].w.squarelength()<0.001f);

			newLambda=lcp_data_contact.right_side_poscorr[constraint_index]-
				dot(jPtr[actconstraint].v1,accPtr[body1].p)-
				dot(jPtr[actconstraint].w1,accPtr[body1].o)-
				dot(jPtr[actconstraint].v2,accPtr[body2].p)-
				dot(jPtr[actconstraint].w2,accPtr[body2].o);

			newLambda*= lcp_data_contact.diag[constraint_index];
			newLambda+=lcp_data_contact.lambda_poscorr[constraint_index];

			if (newLambda<0)
				newLambda=0;

			deltaLambda=newLambda-lcp_data_contact.lambda_poscorr[constraint_index];

			lcp_data_contact.lambda_poscorr[constraint_index]=newLambda;

			accPtr[body1].p+=deltaLambda*bPtr[actconstraint].v1;
			accPtr[body1].o+=deltaLambda*bPtr[actconstraint].w1;

			assertion(accPtr[body1].o.squarelength()<0.001f);

			accPtr[body2].p+=deltaLambda*bPtr[actconstraint].v2;
			accPtr[body2].o+=deltaLambda*bPtr[actconstraint].w2;
			assertion(accPtr[body2].o.squarelength()<0.001f);
		}

		constraint_index=constraint_index_save+4;
	}
}

void lcp_solver_t::solve_frictions()
{
	physicssystem* ptr=physicssystem::ptr;
	accel_t* __restrict accPtr=ptr->bodystate_array.constraint_accel;
	int constraint_index=0;
	for (int actcontact=0; actcontact<contact_count; ++actcontact)
	{
		const int body1=lcp_data_contact.body_index[actcontact].i[0];
		const int body2=lcp_data_contact.body_index[actcontact].i[1];

		const int numconstraint=lcp_data_contact.constraintnum[actcontact];

#ifdef _DEBUG
		const jacobi* __restrict jPtr=&lcp_data_friction.J[constraint_index];
		const jacobi* __restrict bPtr=&lcp_data_friction.B[constraint_index];
#else
		const jacobi* __restrict jPtr=lcp_data_friction.J+constraint_index;
		const jacobi* __restrict bPtr=lcp_data_friction.B+constraint_index;
#endif

		f32 coeff=lcp_data_friction.friction_coeff[actcontact];
		int constraint_index_save=constraint_index;

		for (int actconstraint=0; actconstraint<numconstraint; ++actconstraint)
		{
			f32 newLambda=lcp_data_friction.right_side[constraint_index]-
				dot(jPtr[actconstraint].v1,accPtr[body1].v)-
				dot(jPtr[actconstraint].w1,accPtr[body1].w)-
				dot(jPtr[actconstraint].v2,accPtr[body2].v)-
				dot(jPtr[actconstraint].w2,accPtr[body2].w);

			newLambda*= lcp_data_friction.diag[constraint_index];
			newLambda+=lcp_data_friction.lambda[constraint_index];

			f32 frictionLimit=coeff*lcp_data_contact.lambda[constraint_index];
			newLambda=clamp(newLambda,-frictionLimit,frictionLimit);

			f32 deltaLambdaFriction=newLambda-lcp_data_friction.lambda[constraint_index];

			lcp_data_friction.lambda[constraint_index]=newLambda;

			accPtr[body1].v+=deltaLambdaFriction*bPtr[actconstraint].v1;
			accPtr[body1].w+=deltaLambdaFriction*bPtr[actconstraint].w1;
			accPtr[body2].v+=deltaLambdaFriction*bPtr[actconstraint].v2;
			accPtr[body2].w+=deltaLambdaFriction*bPtr[actconstraint].w2;
			assertion(accPtr[body1].w.squarelength()<0.001f);
			assertion(accPtr[body2].w.squarelength()<0.001f);
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

void lcp_solver_t::process(contact_surface_t** i_contact_array, int i_contact_count, f32 i_dt)
{
	contact_array=i_contact_array;
	contact_count=i_contact_count;
	dt=i_dt;


	allocate_buffer();
	pre_step();
	solve_constraints();
	cache_lambda();
	clean();
}

void lcp_solver_t::pre_step()
{
	pre_step_contacts_and_frictions();
}

void lcp_solver_t::cache_lambda()
{
	for (int n=0; n<contact_count; ++n)
	{
		contact_surface_t* act_contact=contact_array[n];
		
		int constraint_index=4*n;
		for (int m=0;m<act_contact->contact_count; ++m,++constraint_index)
		{
			contact_point_t* cp=act_contact->contactarray+m;
			cp->cached_lambda=lcp_data_contact.lambda[constraint_index];
		}
	}
}

void lcp_solver_t::clean()
{
#if 0
	delete [] lcp_data_contact.J;
	delete [] lcp_data_contact.B;
	delete [] lcp_data_contact.right_side;
	delete [] lcp_data_contact.right_side_poscorr;
	delete [] lcp_data_contact.diag;
	delete [] lcp_data_contact.lambda;
	delete [] lcp_data_contact.lambda_poscorr;
	delete [] lcp_data_contact.constraintnum;
	delete [] lcp_data_contact.body_index;

	delete [] lcp_data_friction.J;
	delete [] lcp_data_friction.B;
	delete [] lcp_data_friction.right_side;
	delete [] lcp_data_friction.diag;
	delete [] lcp_data_friction.lambda;
	delete [] lcp_data_friction.friction_coeff;
#endif
}
