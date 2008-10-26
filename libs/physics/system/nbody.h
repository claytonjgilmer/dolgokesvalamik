#ifndef _nbody_h_
#define _nbody_h_

#include "containers/vector.h"
#include "math/vec3.h"
#include "math/mtx3x3.h"
#include "math/mtx4x3.h"
#include "body.h"

#define NBODY_MIN_CAPACITY 512
	struct nbody
	{
		mtx4x3* pos;
		vec3* vel;
		vec3* rotvel;

		vec3* force;
		vec3* torque;
		float* invmass;
		mtx3x3* invinertia_rel;
		mtx3x3* invinertia_abs;

		body_t** body;

		unsigned capacity;
		unsigned size;

		nbody();
		~nbody();

		void add_body(const bodydesc i_desc[], body_t* i_body_array[], unsigned i_bodynum);
		void release_body(body_t* i_body_array[], unsigned i_bodynum);

		void predict(float i_dt);

		unsigned state_size[100];
		unsigned state_size_sum;
		unsigned state_num;

		void realloc(unsigned i_newcapacity);
	};
#endif//_nbody_h_