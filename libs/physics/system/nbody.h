#ifndef _nbody_h_
#define _nbody_h_

#include "containers/vector.h"
#include "math/vec3.h"
#include "math/mtx3x3.h"
#include "math/mtx4x3.h"
#include "body.h"

#define NBODY_MIN_CAPACITY 512
/*
minek ez a sok szar a statikus testeknek? nekik nem kell, csak pozicio
vagy meg asse
*/
struct accel_t
{
	vec3 v;
	vec3 w;
	vec3 p;
	vec3 o;

	void clear()
	{
		v.clear();
		w.clear();
		p.clear();
		o.clear();
	}
};


	struct nbody_t
	{
		mtx4x3* pos;
		vec3* vel;
		vec3* rotvel;

		vec3* force;
		vec3* torque;
		accel_t* constraint_accel;

		f32* invmass;
		mtx3x3* invinertia_rel;
		mtx3x3* invinertia_abs;

		body_t** body;

		unsigned capacity;
		unsigned size;

		nbody_t();
		~nbody_t();

		void add_world();
		void add_body(const bodydesc i_desc[], body_t* i_body_array[], unsigned i_bodynum);
		void release_body(body_t* i_body_array[], unsigned i_bodynum);

		void predict(f32 i_dt);

		unsigned state_size[100];
		unsigned state_size_sum;
		unsigned state_num;

	};
#endif//_nbody_h_
