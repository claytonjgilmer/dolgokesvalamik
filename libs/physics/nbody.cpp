#include "nbody.h"

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
	state_num(0)
	{
		DECLARE_STATE(pos,mtx4x3);
		DECLARE_STATE(vel,vec3);
		DECLARE_STATE(rotvel,vec3);
		DECLARE_STATE(force,vec3);
		DECLARE_STATE(torque,vec3);
		DECLARE_STATE(invmass,float);
		DECLARE_STATE(invinertia_rel,mtx3x3);
		DECLARE_STATE(invinertia_abs,mtx3x3);
		DECLARE_STATE(body,body_t*);

		size=0;
		capacity=0;

		realloc(NBODY_MIN_CAPACITY);
	}

	nbody::~nbody()
	{
		void* firstaddress=*((void**)this);
		free(firstaddress);
	}

	void nbody::realloc(unsigned i_newcapacity)
	{
		char* newbuf=(char*)malloc(state_size_sum*i_newcapacity);

		if (size)
		{
			unsigned act_offset=0;
			for (unsigned n=0; n<state_num; ++n)
			{
				act_offset+=state_size[n]*capacity;
				void* srcaddress=(void*)*(((char*)this)+4*n);
				memcpy(newbuf+act_offset,srcaddress,size*state_size[n]);
			}
		}

		for (unsigned n=0; n<state_num; ++n)
		{
			void* &srcaddress=(void*&)*(((char*)this)+4*n);
			srcaddress=newbuf+4*n;
		}

		capacity=i_newcapacity;
	}

	void nbody::add_body(const bodydesc* i_desc, body_t* i_body_array[], unsigned i_bodynum)
	{
		if (size+i_bodynum>capacity)
			realloc(nextpoweroftwo(size+i_bodynum));

		for (unsigned n=0,index=size; n<i_bodynum;++n,++index)
		{
			i_body_array[n]->index=index;
			pos[index]=i_desc[n].pos;
			vel[index]=i_desc[n].vel;
			rotvel[index]=i_desc[n].rotvel;

			if (i_desc[n].isstatic)
			{
				invmass[index]=0;
				invinertia_rel[index].clear();
			}
			else
			{
				invmass[index]=1/i_desc[n].mass;
				invinertia_rel[index].invert3x3(i_desc[n].inertia);
			}
		}
		
	}

}//namespace