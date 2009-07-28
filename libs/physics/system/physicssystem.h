#ifndef _physicssystem_h_
#define _physicssystem_h_

#include "utils/singleton.h"
#include "containers/listallocator.h"
#include "math/mtx4x3.h"
#include "body.h"
#include "nbody.h"
#include "contactgroupmanager.h"
#include "physics/collision/system/contactmanager.h"
#include "physics/collision/shapes/shape.h"
#include "physics/collision/shapeintersection/shapeintersection.h"
#include "physics/collision/broadphase/hgridbroadphase.h"

//extern unsigned g_bph,g_nph,g_in,g_up,g_frc,g_sol,g_presol,g_solcon,g_solv_cont,g_solv_fric,g_cache_l;


	struct physicssystemdesc
	{
		vec3 gravity;
		f32 timestep;
		uint8 maxsubstepcount;
		uint8 parallel_processing;
		f32 solver_positioncorrection_rate;
		f32 solver_lambda_relaxation;
		f32 solver_max_penetration;

		physicssystemdesc()
		{
			gravity.set(0,-10,0);
			timestep=0.01f;
			maxsubstepcount=10;
			parallel_processing=0;
			solver_positioncorrection_rate=0.1f;
			solver_lambda_relaxation=0.9f;
			solver_max_penetration=.01f;
		}
	};


	typedef int (*intersectfn)(shape_t* i_sph1, const mtx4x3& i_body1_mtx, shape_t* i_sph2, const mtx4x3& i_body2_mtx, vec3 o_contact_array[][2], vec3& o_normal, uint32& o_contact_num);

	struct physicssystem
	{
		//body krealas/megszuntetes
		body_t* create_body(const bodydesc& i_desc);
		void create_bodies(body_t* o_body_array[], const bodydesc i_desc[], unsigned i_bodynum);
		void release_body(body_t*);
		void release_bodies(body_t* i_body_array[], unsigned i_bodynum);


		//eztet kell hivni
		void simulate(f32 i_dt);

		body_t* world;

		void set_gravity(const vec3& gravity)
		{
			desc.gravity=gravity;
		}






		physicssystem(const physicssystemdesc* i_desc);

		physicssystemdesc desc;
		nbody_t bodystate_array;

		list_allocator<sizeof(body_t)> body_list;
		vector<body_t*> killed;

		contactmanager contact_manager;
		contact_group_manager_t contact_group_manager;
		intersectfn intersect_fn[shape_type_num][shape_type_num];
		hgridbroadphase broad_phase;

//		uint32 parallel_processing;

		uint32 parallel_boudingupdate;
		uint32 parallel_broadphase;
		uint32 parallel_nearphase;
		uint32 parallel_update;
		uint32 parallel_inertia;
		uint32 parallel_pre_step;
		uint32 parallel_solver;
		int frame_count;
		f32 solver_position_correction_rate;

		DECLARE_SINGLETON_DESC(physicssystem,physicssystemdesc);
	};
#endif//_physicssystem_h_
