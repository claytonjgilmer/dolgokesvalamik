#ifndef _scratchpad_h_
#define _scratchpad_h_

#include "math/math.h"

template <uint32 capacity>
struct scratch_pad_t
{
	scratch_pad_t()
	{
		ptr=0;
	}
	char* alloc(uint32 size)
	{
		return buf+InterlockedExchangeAdd(&ptr,size);
	}

	void flush()
	{
		ptr=0;
	}

	char buf[capacity];

	volatile long ptr;
};

#endif//_scratchpad_h_