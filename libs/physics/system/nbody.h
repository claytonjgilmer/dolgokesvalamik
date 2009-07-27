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


#define SOA


#ifndef SOA
struct body_data_t
{
	mtx4x3 pos;
	vec3 vel;
	vec3 rotvel;

	vec3 force;
	vec3 torque;
	accel_t constraint_accel;

	f32 invmass;
	mtx3x3 invinertia_rel;
	mtx3x3 invinertia_abs;

	body_t* body;
};
#endif




struct nbody_t
{
	nbody_t();
	~nbody_t();

	void add_world(body_t*);
	void add_body(const bodydesc i_desc[], body_t* i_body_array[], unsigned i_bodynum);
	void release_body(body_t* i_body_array[], unsigned i_bodynum);

//	void predict(f32 i_dt);

	mtx4x3& get_pos(int32 index);
	vec3& get_vel(int32 index);
	vec3& get_rotvel(int32 index);
	vec3& get_force(int32 index);
	vec3& get_torque(int32 index);
	accel_t& get_constraint_accel(int32 index);
	f32& get_invmass(int32 index);
	mtx3x3& get_invinertia_rel(int32 index);
	mtx3x3& get_invinertia_abs(int32 index);
	body_t*& get_body(int32 index);

	uint32 get_size() const;


//private:
#ifdef SOA
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

	unsigned state_size[100];
	unsigned state_size_sum;
	unsigned state_num;
#else
	vector<body_data_t> body_data;
#endif
};

#ifdef SOA
__forceinline mtx4x3& nbody_t::get_pos(int32 index)
{
	return this->pos[index];
}
__forceinline vec3& nbody_t::get_vel(int32 index)
{
	return this->vel[index];
}
__forceinline vec3& nbody_t::get_rotvel(int32 index)
{
	return this->rotvel[index];
}
__forceinline vec3& nbody_t::get_force(int32 index)
{
	return this->force[index];
}
__forceinline vec3& nbody_t::get_torque(int32 index)
{
	return this->torque[index];
}
__forceinline accel_t& nbody_t::get_constraint_accel(int32 index)
{
	return this->constraint_accel[index];
}
__forceinline f32& nbody_t::get_invmass(int32 index)
{
	return this->invmass[index];
}
__forceinline mtx3x3& nbody_t::get_invinertia_rel(int32 index)
{
	return this->invinertia_rel[index];
}
__forceinline mtx3x3& nbody_t::get_invinertia_abs(int32 index)
{
	return this->invinertia_abs[index];
}
__forceinline body_t*& nbody_t::get_body(int32 index)
{
	return this->body[index];
}

__forceinline uint32 nbody_t::get_size() const
{
	return size;
}
#else
__forceinline mtx4x3& nbody_t::get_pos(int32 index)
{
	return body_data[index].pos;
}
__forceinline vec3& nbody_t::get_vel(int32 index)
{
	return body_data[index].vel;
}
__forceinline vec3& nbody_t::get_rotvel(int32 index)
{
	return body_data[index].rotvel;
}
__forceinline vec3& nbody_t::get_force(int32 index)
{
	return body_data[index].force;
}
__forceinline vec3& nbody_t::get_torque(int32 index)
{
	return body_data[index].torque;
}
__forceinline accel_t& nbody_t::get_constraint_accel(int32 index)
{
	return body_data[index].constraint_accel;
}
__forceinline f32& nbody_t::get_invmass(int32 index)
{
	return body_data[index].invmass;
}
__forceinline mtx3x3& nbody_t::get_invinertia_rel(int32 index)
{
	return body_data[index].invinertia_rel;
}
__forceinline mtx3x3& nbody_t::get_invinertia_abs(int32 index)
{
	return body_data[index].invinertia_abs;
}
__forceinline body_t*& nbody_t::get_body(int32 index)
{
	return body_data[index].body;
}
__forceinline uint32 nbody_t::get_size() const
{
	return body_data.size();
}

#endif


#endif//_nbody_h_
