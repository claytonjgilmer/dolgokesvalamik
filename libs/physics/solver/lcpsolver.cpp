#include "lcpsolver.h"
#include "threading/taskmanager.h"
#include "utils/timer.h"


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

#ifdef use_vector
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
#else
	uint32 mem_to_alloc=sizeof(jacobi)*(4*contact_count+4*contact_count+4*contact_count+4*contact_count)+
		sizeof(f32)*(4*contact_count+4*contact_count+4*contact_count+4*contact_count+4*contact_count+4*contact_count+4*contact_count+4*contact_count+contact_count)+
		sizeof(uint32)*contact_count+
		sizeof(body_index_t)*contact_count;

	char* mem=(char*)malloc(mem_to_alloc);
	lcp_data_contact.J=(jacobi*)mem; mem+=sizeof(jacobi)*4*contact_count;
	lcp_data_contact.B=(jacobi*)mem; mem+=sizeof(jacobi)*4*contact_count;

	lcp_data_contact.right_side=(f32*)mem; mem+=sizeof(f32)*4*contact_count;
	lcp_data_contact.right_side_poscorr=(f32*)mem; mem+=sizeof(f32)*4*contact_count;
	lcp_data_contact.diag=(f32*)mem; mem+=sizeof(f32)*4*contact_count;
	lcp_data_contact.lambda=(f32*)mem; mem+=sizeof(f32)*4*contact_count;
	lcp_data_contact.lambda_poscorr=(f32*)mem; mem+=sizeof(f32)*4*contact_count;

	lcp_data_contact.constraintnum=(uint32*)mem; mem+=sizeof(uint32)*contact_count;
	lcp_data_contact.body_index=(body_index_t*)mem; mem+=sizeof(body_index_t)*contact_count;

	lcp_data_friction.J=(jacobi*)mem; mem+=sizeof(jacobi)*4*contact_count;
	lcp_data_friction.B=(jacobi*)mem; mem+=sizeof(jacobi)*4*contact_count;

	lcp_data_friction.right_side=(f32*)mem; mem+=sizeof(f32)*4*contact_count;
	lcp_data_friction.diag=(f32*)mem; mem+=sizeof(f32)*4*contact_count;
	lcp_data_friction.lambda=(f32*)mem; mem+=sizeof(f32)*4*contact_count;
	lcp_data_friction.friction_coeff=(f32*)mem;
//	lcp_data_contact.J=new jacobi[4*contact_count];
//	lcp_data_contact.B=new jacobi[4*contact_count];

//	lcp_data_contact.right_side=new f32[4*contact_count];
//	lcp_data_contact.right_side_poscorr=new f32[4*contact_count];
//	lcp_data_contact.diag=new f32[4*contact_count];
//	lcp_data_contact.lambda=new f32[4*contact_count];
//	lcp_data_contact.lambda_poscorr=new f32[4*contact_count];

//	lcp_data_contact.constraintnum=new uint32[contact_count];
//	lcp_data_contact.body_index=new body_index_t[contact_count];

//	lcp_data_friction.J=new jacobi[4*contact_count];
//	lcp_data_friction.B=new jacobi[4*contact_count];

//	lcp_data_friction.right_side=new f32[4*contact_count];
//	lcp_data_friction.diag=new f32[4*contact_count];
//	lcp_data_friction.lambda=new f32[4*contact_count];

//	lcp_data_friction.friction_coeff=new f32[contact_count];
#endif
}

struct solver_pre_step_contacts
{
	lcp_solver_t* solver;
	solver_pre_step_contacts(lcp_solver_t* i_solver): solver(i_solver){}

	void operator()(unsigned start, unsigned count)
	{
		int constraint_index=solver->contact_index[start];
		physicssystem* ptr=physicssystem::ptr;
		f32 pos_corr_rate=ptr->solver_position_correction_rate;
		f32 relax=ptr->desc.solver_lambda_relaxation;
		f32 max_penetration=ptr->desc.solver_max_penetration;
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
			for (int k=0; k<act_contact->contact_count; ++k,++constraint_index)
			{
				contact_point_t* act_cp=act_contact->contactarray+k;
				lcp_data->lambda[constraint_index]=relax*act_cp->cached_lambda;
				lcp_data->lambda_poscorr[constraint_index]=0;

				vec3 relpos1=act_cp->abs_pos[0]-nbody->get_pos(body1).t;
				vec3 relpos2=act_cp->abs_pos[1]-nbody->get_pos(body2).t;

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

				b->v1=nbody->get_invmass(body1)*j->v1;
				b->w1=nbody->get_invinertia_abs(body1).transform3x3(j->w1);
				b->v2=nbody->get_invmass(body2)*j->v2;
				b->w2=nbody->get_invinertia_abs(body2).transform3x3(j->w2);

				const f32 lambda=solver->lcp_data_contact.lambda[constraint_index];
				nbody->get_constraint_accel(body1).v+=lambda*b->v1;
				nbody->get_constraint_accel(body1).w+=lambda*b->w1;

				nbody->get_constraint_accel(body2).v+=lambda*b->v2;
				nbody->get_constraint_accel(body2).w+=lambda*b->w2;



				vec3 dv=	nbody->get_vel(body2)+
					cross(nbody->get_rotvel(body2), relpos2) -
					nbody->get_vel(body1)-
					cross(nbody->get_rotvel(body1), relpos1);

				f32 vel_normal = dot(dv, act_contact->normal);
				f32 biasVelocityPositionCorrection=-pos_corr_rate*min(act_cp->penetration+max_penetration,0.0f)/solver->dt;
				f32 restitution=-min(vel_normal*act_contact->restitution+0.05f,0.0f);

				lcp_data->right_side[constraint_index]=restitution-
					(dot(j->v1,nbody->get_vel(body1))+
					dot(j->w1,nbody->get_rotvel(body1))+
					dot(j->v2,nbody->get_vel(body2))+
					dot(j->w2,nbody->get_rotvel(body2)));

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

				fb->v1=nbody->get_invmass(body1)*fj->v1;
				nbody->get_invinertia_abs(body1).transform3x3(fb->w1,fj->w1);
				fb->v2=nbody->get_invmass(body2)*fj->v2;
				nbody->get_invinertia_abs(body2).transform3x3(fb->w2,fj->w2);

				lcp_data_friction->right_side[constraint_index]=-
					(dot(fj->v1,nbody->get_vel(body1))+
					dot(fj->w1,nbody->get_rotvel(body1))+
					dot(fj->v2,nbody->get_vel(body2))+
					dot(fj->w2,nbody->get_rotvel(body2)));// /i_DeltaTime;



				lcp_data_friction->diag[constraint_index]=1.0f/
					(dot(fj->v1,fb->v1)+
					dot(fj->w1,fb->w1)+
					dot(fj->v2,fb->v2)+
					dot(fj->w2,fb->w2));
			}
		}
	}
};

uint32 constraint_c=0;

void lcp_solver_t::pre_step_contacts_and_frictions()
{
	body_count=0;
	physicssystem* ptr=physicssystem::ptr;
	
	int i=0;
	for (int n=0; n<this->contact_count; ++n)
	{
		this->contact_index[n]=i;
		i+=this->contact_array[n]->contact_count;
	}

	this->constraint_count=i;
	constraint_c=i;

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
#ifdef _DEBUG
		const jacobi* __restrict jPtr=&lcp_data_contact.J[0];
		const jacobi* __restrict bPtr=&lcp_data_contact.B[0];
#else
		const jacobi* __restrict jPtr=lcp_data_contact.J;
		const jacobi* __restrict bPtr=lcp_data_contact.B;
#endif

	for (int actcontact=0; actcontact<contact_count; ++actcontact)
	{
		const int body1=lcp_data_contact.body_index[actcontact].i[0];
		const int body2=lcp_data_contact.body_index[actcontact].i[1];
		accel_t& acc_body1=ptr->bodystate_array.get_constraint_accel(body1);
		accel_t& acc_body2=ptr->bodystate_array.get_constraint_accel(body2);

		const int numconstraint=lcp_data_contact.constraintnum[actcontact];


		for (int actconstraint=0; actconstraint<numconstraint; ++actconstraint,++constraint_index,++jPtr,++bPtr)
		{
			f32 newLambda=lcp_data_contact.right_side[constraint_index]-
				dot(jPtr->v1,acc_body1.v)-
				dot(jPtr->w1,acc_body1.w)-
				dot(jPtr->v2,acc_body2.v)-
				dot(jPtr->w2,acc_body2.w);

			newLambda*= lcp_data_contact.diag[constraint_index];
			newLambda+=lcp_data_contact.lambda[constraint_index];

			if (newLambda<0)
				newLambda=0;

			f32 deltaLambda=newLambda-lcp_data_contact.lambda[constraint_index];
			lcp_data_contact.lambda[constraint_index]=newLambda;

			acc_body1.v+=deltaLambda* bPtr->v1;
			acc_body1.w+=deltaLambda* bPtr->w1;

			acc_body2.v+=deltaLambda* bPtr->v2;
			acc_body2.w+=deltaLambda* bPtr->w2;

			newLambda=lcp_data_contact.right_side_poscorr[constraint_index]-
				dot(jPtr->v1,acc_body1.p)-
				dot(jPtr->w1,acc_body1.o)-
				dot(jPtr->v2,acc_body2.p)-
				dot(jPtr->w2,acc_body2.o);

			newLambda*= lcp_data_contact.diag[constraint_index];
			newLambda+=lcp_data_contact.lambda_poscorr[constraint_index];

			if (newLambda<0)
				newLambda=0;

			deltaLambda=newLambda-lcp_data_contact.lambda_poscorr[constraint_index];

			lcp_data_contact.lambda_poscorr[constraint_index]=newLambda;

			acc_body1.p+=deltaLambda*bPtr->v1;
			acc_body1.o+=deltaLambda*bPtr->w1;

			acc_body2.p+=deltaLambda*bPtr->v2;
			acc_body2.o+=deltaLambda*bPtr->w2;
		}
	}
}

void lcp_solver_t::solve_frictions()
{
	physicssystem* ptr=physicssystem::ptr;
	nbody_t* b=&ptr->bodystate_array;
	int constraint_index=0;
#ifdef _DEBUG
		const jacobi* __restrict jPtr=&lcp_data_friction.J[0];
		const jacobi* __restrict bPtr=&lcp_data_friction.B[0];
#else
		const jacobi* __restrict jPtr=lcp_data_friction.J;
		const jacobi* __restrict bPtr=lcp_data_friction.B;
#endif

	for (int actcontact=0; actcontact<contact_count; ++actcontact)
	{
		const int body1=lcp_data_contact.body_index[actcontact].i[0];
		const int body2=lcp_data_contact.body_index[actcontact].i[1];
		accel_t& acc_body1=b->get_constraint_accel(body1);
		accel_t& acc_body2=b->get_constraint_accel(body2);

		const int numconstraint=lcp_data_contact.constraintnum[actcontact];

		f32 coeff=lcp_data_friction.friction_coeff[actcontact];
		for (int actconstraint=0; actconstraint<numconstraint; ++actconstraint,++constraint_index,++jPtr,++bPtr)
		{
			f32 newLambda=lcp_data_friction.right_side[constraint_index]-
				dot(jPtr->v1,acc_body1.v)-
				dot(jPtr->w1,acc_body1.w)-
				dot(jPtr->v2,acc_body2.v)-
				dot(jPtr->w2,acc_body2.w);

			newLambda*= lcp_data_friction.diag[constraint_index];
			newLambda+=lcp_data_friction.lambda[constraint_index];

			f32 frictionLimit=coeff*lcp_data_contact.lambda[constraint_index];
			newLambda=clamp(newLambda,-frictionLimit,frictionLimit);

			f32 deltaLambdaFriction=newLambda-lcp_data_friction.lambda[constraint_index];

			lcp_data_friction.lambda[constraint_index]=newLambda;

			acc_body1.v+=deltaLambdaFriction*bPtr->v1;
			acc_body1.w+=deltaLambdaFriction*bPtr->w1;
			acc_body2.v+=deltaLambdaFriction*bPtr->v2;
			acc_body2.w+=deltaLambdaFriction*bPtr->w2;
		}
	}

}

void lcp_solver_t::solve_constraints()
{
	g_solv_cont=g_solv_fric=0;
	timer_t t;
	for (int it=0; it<10; ++it)
	{
		t.reset();
		solve_contacts();
		t.stop();
		g_solv_cont+=t.get_tick();
		t.reset();
		solve_frictions();
		t.stop();
		g_solv_fric+=t.get_tick();
	}
}

extern unsigned g_presol,g_solcon;

void lcp_solver_t::process(contact_surface_t** i_contact_array, int i_contact_count, f32 i_dt)
{
	contact_array=i_contact_array;
	contact_count=i_contact_count;
	dt=i_dt;

	timer_t t;

	t.reset();
	allocate_buffer();
	t.stop();
	g_cache_l=t.get_tick();


	t.reset();
	pre_step();
	t.stop();
	g_presol=t.get_tick();
	t.reset();
	solve_constraints();
	t.stop();
	g_solcon=t.get_tick();
	cache_lambda();
	clean();
}

void lcp_solver_t::pre_step()
{
	pre_step_contacts_and_frictions();
}

void lcp_solver_t::cache_lambda()
{
	int constraint_index=0;
	for (int n=0; n<contact_count; ++n)
	{
		contact_surface_t* act_contact=contact_array[n];
		
		for (int m=0;m<act_contact->contact_count; ++m,++constraint_index)
		{
			contact_point_t* cp=act_contact->contactarray+m;
			cp->cached_lambda=lcp_data_contact.lambda[constraint_index];
		}
	}
}

void lcp_solver_t::clean()
{
#ifndef use_vector
/*
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
*/
	free(lcp_data_contact.J);
#endif
}
