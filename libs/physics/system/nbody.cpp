#include "nbody.h"

#ifdef SOA
#ifndef offsetof
#define offsetof(_s_,_m_) (size_t)(&(((_s_*)0)->_m_))
#endif

#define DECLARE_STATE(_name_,_type_)	state_size[state_num]=sizeof(_type_); \
										state_size_sum+=sizeof(_type_); \
										state_num++


void realloc(nbody_t* nb, unsigned i_newcapacity)
{
	void* firstaddress=*((void**)nb);
	nb->capacity=i_newcapacity;
	char* newbuf=(char*)malloc(nb->state_size_sum*i_newcapacity);

	if (nb->size)
	{
		unsigned act_offset=0;
		for (unsigned n=0; n<nb->state_num; ++n)
		{
			void* srcaddress=*(((void**)nb)+n);
			memcpy(newbuf+act_offset,srcaddress,nb->size*nb->state_size[n]);
			act_offset+=nb->state_size[n]*nb->capacity;
		}
	}

	unsigned act_offset=0;
	for (unsigned n=0; n<nb->state_num; ++n)
	{
		void* &srcaddress=(void*&)*(((char*)nb)+4*n);
		srcaddress=newbuf+act_offset;
		act_offset+=nb->state_size[n]*nb->capacity;
	}
	if (firstaddress)
		free(firstaddress);
}



	nbody_t::nbody_t():
	state_size_sum(0),
	state_num(0),
	size(0),
	capacity(0)
	{
		DECLARE_STATE(this->pos,mtx4x3);
		DECLARE_STATE(this->vel,vec3);
		DECLARE_STATE(this->rotvel,vec3);
		DECLARE_STATE(this->force,vec3);
		DECLARE_STATE(this->torque,vec3);
		DECLARE_STATE(this->constraint_accel,accel_t);
		DECLARE_STATE(this->invmass,f32);
		DECLARE_STATE(this->invinertia_rel,mtx3x3);
		DECLARE_STATE(this->invinertia_abs,mtx3x3);
		DECLARE_STATE(this->body,body_t*);

		pos=NULL;
		realloc(this,NBODY_MIN_CAPACITY);
	}

	nbody_t::~nbody_t()
	{
		void* firstaddress=*((void**)this);
		free(firstaddress);
	}

	void nbody_t::add_world()
	{
		if (size+1>capacity)
			realloc(this,nextpoweroftwo(size));

		g_world->array_index=0;
		pos[0].identity();
		vel[0].clear();
		rotvel[0].clear();
		invmass[0]=0;
		invinertia_rel[0].clear();
		invinertia_abs[0].clear();
		force[0].clear();
		torque[0].clear();
		body[0]=g_world;
		constraint_accel[0].clear();

		++size;
	}

	void nbody_t::add_body(const bodydesc i_desc[], body_t* i_body_array[], unsigned i_bodynum)
	{
		if (size+i_bodynum>capacity)
			realloc(this,nextpoweroftwo(size+i_bodynum));

		for (unsigned n=0,index=size; n<i_bodynum;++n,++index)
		{
			i_body_array[n]->array_index=index;
			this->pos[index]=i_desc[n].pos;
			this->vel[index]=i_desc[n].vel;
			this->rotvel[index]=i_desc[n].rotvel;

			this->invmass[index]=1/i_desc[n].mass;
			this->invinertia_rel[index].invert3x3(i_desc[n].inertia);
			this->force[index].clear();
			this->torque[index].clear();
			this->constraint_accel[index].clear();
			this->body[index]=i_body_array[n];
		}

		size+=i_bodynum;

	}

	void nbody_t::release_body(body_t* i_body_array[], unsigned i_bodynum)
	{
		for (uint32 n=0; n<i_bodynum; ++n)
		{
			--this->size;
			uint16 index=i_body_array[n]->array_index;

			this->pos[index]=this->pos[this->size];
			this->vel[index]=this->vel[this->size];
			this->rotvel[index]=this->rotvel[this->size];
			this->force[index]=this->force[this->size];
			this->torque[index]=this->torque[this->size];
			//constraint_accel egy frame-ig el, nem kell masolni
			this->invmass[index]=this->invmass[this->size];
			this->invinertia_rel[index]=this->invinertia_rel[this->size];//ugy gondolom az abszolut inerciat nem kell masolni, mert menet kozben nem torlunk
			this->body[index]=this->body[this->size];
			this->body[index]->array_index=index;
		}
	}
#else
nbody_t::nbody_t()
{
	body_data.reserve(NBODY_MIN_CAPACITY);
}

nbody_t::~nbody_t(){}



void nbody_t::add_world()
{
	assertion(!body_data.size());
	body_data.push_back(body_data_t());

	g_world->array_index=0;
	this->
	body_data[0].pos.identity();
	body_data[0].vel.clear();
	body_data[0].rotvel.clear();
	body_data[0].invmass=0;
	body_data[0].invinertia_rel.clear();
	body_data[0].invinertia_abs.clear();
	body_data[0].force.clear();
	body_data[0].torque.clear();
	body_data[0].body=g_world;
	body_data[0].constraint_accel.clear();
}

void nbody_t::add_body(const bodydesc i_desc[], body_t* i_body_array[], unsigned i_bodynum)
{
	for (unsigned n=0,index=body_data.size(); n<i_bodynum;++n,++index)
	{
		body_data.push_back(body_data_t());
		i_body_array[n]->array_index=index;
		body_data[index].pos=i_desc[n].pos;
		body_data[index].vel=i_desc[n].vel;
		body_data[index].rotvel=i_desc[n].rotvel;

		body_data[index].invmass=1/i_desc[n].mass;
		body_data[index].invinertia_rel.invert3x3(i_desc[n].inertia);
		body_data[index].force.clear();
		body_data[index].torque.clear();
		body_data[index].constraint_accel.clear();
		body_data[index].body=i_body_array[n];
	}
}

void nbody_t::release_body(body_t* i_body_array[], unsigned i_bodynum)
{
	for (uint32 n=0; n<i_bodynum; ++n)
	{
		int32 size=body_data.size()-1;
		uint16 index=i_body_array[n]->array_index;

		body_data[index].pos=body_data[size].pos;
		body_data[index].vel=body_data[size].vel;
		body_data[index].rotvel=body_data[size].rotvel;
		body_data[index].force=body_data[size].force;
		body_data[index].torque=body_data[size].torque;
		//constraint_accel egy frame-ig el, nem kell masolni
		body_data[index].invmass=body_data[size].invmass;
		body_data[index].invinertia_rel=body_data[size].invinertia_rel;//ugy gondolom az abszolut inerciat nem kell masolni, mert menet kozben nem torlunk
		body_data[index].body=body_data[size].body;
		body_data[index].body->array_index=index;
		body_data.pop_back();
	}
}

#endif
