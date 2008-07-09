#ifndef _assert_h_
#define _assert_h_

#include <assert.h>
#include <stdio.h>
#include <Windows.h>

namespace utils
{
	inline void assertion(bool i_condition, const char* i_msg)
	{
		if (!i_condition)
		{
			OutputDebugString(i_msg);
			OutputDebugString("\n");
			__asm int 3;
		}
	}

}
#endif//_assert_h_