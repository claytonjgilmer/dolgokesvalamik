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
			__asm int 3;
//			__asm (  "int $3");
		}
	}
	inline void assertion(int i_condition, const char* i_msg)
	{
		if (!i_condition)
		{
			OutputDebugString(i_msg);
			OutputDebugString("\n");
			__asm int 3;
//			__asm (  "int $3");
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
