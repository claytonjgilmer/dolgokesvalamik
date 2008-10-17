#include "nbody.h"
#include "contact.h"

#ifndef offsetof
#define offsetof(_s_,_m_) (size_t)(&(((_s_*)0)->_m_))
#endif

namespace physics
{
	using namespace math;


#define DECLARE_STATE(_name_,_type_)	state_size[state_num]=sizeof(_type_); \
										state_size_sum+=sizeof(_type_); \
										state_num++

	nbody::nbody():
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

		this->realloc(NBODY_MIN_CAPACITY);
	}

	nbody::~nbody()
	{
		void* firstaddress=*((void**)this);
		free(firstaddress);
	}

	void nbody::realloc(unsigned i_newcapacity)
	{
		char* newbuf=(char*)malloc(this->state_size_sum*i_newcapacity);

		if (size)
		{
			unsigned act_offset=0;
			for (unsigned n=0; n<this->state_num; ++n)
			{
				act_offset+=this->state_size[n]*this->capacity;
				void* srcaddress=(void*)*(((char*)this)+4*n);
				memcpy(newbuf+act_offset,srcaddress,size*this->state_size[n]);
			}
		}

		for (unsigned n=0; n<state_num; ++n)
		{
			void* &srcaddress=(void*&)*(((char*)this)+4*n);
			srcaddress=newbuf+4*n;
		}

		this->capacity=i_newcapacity;
	}

	void nbody::add_body(const bodydesc i_desc[], body_t* i_body_array[], unsigned i_bodynum)
	{
		if (size+i_bodynum>capacity)
			realloc(nextpoweroftwo(size+i_bodynum));

		size+=i_bodynum;

		for (unsigned n=0,index=size; n<i_bodynum;++n,++index)
		{
			i_body_array[n]->array_index=index;
			this->pos[index]=i_desc[n].pos;
			this->vel[index]=i_desc[n].vel;
			this->rotvel[index]=i_desc[n].rotvel;

			if (i_desc[n].is_static)
			{
				this->invmass[index]=0;
				this->invinertia_rel[index].clear();
			}
			else
			{
				this->invmass[index]=1/i_desc[n].mass;
				this->invinertia_rel[index].invert3x3(i_desc[n].inertia);
			}

			this->force[index].clear();
			this->torque[index].clear();
			this->body[index]=i_body_array[n];
		}
		
	}

	void nbody::release_body(body_t* i_body_array[], unsigned i_bodynum)
	{
		for (uint32 n=0; n<i_bodynum; ++n)
		{
			--this->size;
			UINT16 index=i_body_array[n]->array_index;

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

}//namespace