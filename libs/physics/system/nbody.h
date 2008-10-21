#ifndef _nbody_h_
#define _nbody_h_

#include "containers/vector.h"
#include "math/vec3.h"
#include "math/mtx3x3.h"
#include "math/mtx4x3.h"
#include "body.h"

namespace physics
{
#define NBODY_MIN_CAPACITY 512
	struct nbody
	{
		math::mtx4x3* pos;
		math::vec3* vel;
		math::vec3* rotvel;

		math::vec3* force;
		math::vec3* torque;
		float* invmass;
		math::mtx3x3* invinertia_rel;
		math::mtx3x3* invinertia_abs;

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
}
#endif//_nbody_h_