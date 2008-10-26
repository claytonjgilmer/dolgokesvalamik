#ifndef _physicssystem_h_
#define _physicssystem_h_

#include "utils/singleton.h"
#include "containers/listallocator.h"
#include "math/mtx4x3.h"
#include "body.h"
#include "nbody.h"
#include "physics/collision/system/contactmanager.h"
#include "physics/collision/shapes/shape.h"
#include "physics/collision/shapeintersection/shapeintersection.h"
#include "physics/collision/broadphase/hgridbroadphase.h"

	struct physicssystemdesc
	{
		vec3 gravity;
		float timestep;
		uint8 maxsubstepcount;

		physicssystemdesc()
		{
			gravity.set(0,-10,0);
			timestep=0.01f;
			maxsubstepcount=10;
		}
	};


	typedef int (*intersectfn)(shape_t* i_sph1, const mtx4x3& i_body1_mtx, shape_t* i_sph2, const mtx4x3& i_body2_mtx, vec3 o_contact_array[][2], vec3& o_normal, uint32& o_contact_num);

	struct physicssystem
	{
		//body krealas/megszuntetes
		body_t* create_body(const bodydesc& i_desc);
		void create_bodies(body_t* o_body_array[], bodystate i_state, const bodydesc i_desc[], unsigned i_bodynum);
		void release_body(body_t*);
		void release_bodies(body_t* i_body_array[], bodystate i_state, unsigned i_bodynum);


		//eztet kell hivni
		void simulate(float i_dt);






		physicssystem(const physicssystemdesc* i_desc);

		physicssystemdesc desc;
		nbody bodystate_array[2];

		listallocator<body_t> body_list;
		vector<body_t*> killed[2];

		contactmanager cm;
		intersectfn intersect_fn[shape_type_num][shape_type_num];
		hgridbroadphase broad_phase;

		uint32 parallel_processing;

		DECLARE_SINGLETON_DESC(physicssystem,physicssystemdesc);
	};
#endif//_physicssystem_h_