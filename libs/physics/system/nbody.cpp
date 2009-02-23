#include "nbody.h"

#ifndef offsetof
#define offsetof(_s_,_m_) (size_t)(&(((_s_*)0)->_m_))
#endif

#define DECLARE_STATE(_name_,_type_)	state_size[state_num]=sizeof(_type_); \
										state_size_sum+=sizeof(_type_); \
										state_num++


void realloc(nbody_t*, unsigned i_newcapacity);


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
		DECLARE_STATE(this->invmass,float);
		DECLARE_STATE(this->invinertia_rel,mtx3x3);
		DECLARE_STATE(this->invinertia_abs,mtx3x3);
		DECLARE_STATE(this->body,body_t*);

		realloc(this,NBODY_MIN_CAPACITY);
	}

	nbody_t::~nbody_t()
	{
		void* firstaddress=*((void**)this);
		free(firstaddress);
	}

	void realloc(nbody_t* nb, unsigned i_newcapacity)
	{
		nb->capacity=i_newcapacity;
		char* newbuf=(char*)malloc(nb->state_size_sum*i_newcapacity);

		if (nb->size)
		{
			unsigned act_offset=0;
			for (unsigned n=0; n<nb->state_num; ++n)
			{
//				void* srcaddress=(void*)*(((char*)this)+4*n);
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
		force[0].clear();
		torque[0].clear();
		body[0]=g_world;

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
			this->invmass[index]=this->invmass[this->size];
			this->invinertia_rel[index]=this->invinertia_rel[this->size];//ugy gondolom az abszolut inerciat nem kell masolni, mert menet kozben nem torlunk
			this->body[index]=this->body[this->size];
			this->body[index]->array_index=index;
		}
	}
