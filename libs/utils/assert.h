#ifndef _assert_h_
#define _assert_h_

#include <assert.h>
#include <stdio.h>
#include <Windows.h>

#ifdef NEED_ASSERT
	inline void assertion(int i_condition)
	{
		if (!i_condition)
		{
		    #ifdef _MSC_VER
				__asm int 3;
			#else
			__asm (  "int $3");
			#endif
		}
	}
	inline void assertion(int i_condition, const char* i_msg)
	{
		if (!i_condition)
		{
			OutputDebugString(i_msg);
			OutputDebugString("\n");
			#ifdef _MSC_VER
			__asm int 3;
			#else
			__asm (  "int $3");
			#endif
		}
	}
#else
	inline void assertion(int i_condition)
	{
	}
	inline void assertion(int i_condition, const char* i_msg)
	{
	}
#endif
#endif//_assert_h_
