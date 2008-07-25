#include "misc.h"

#include <Windows.h>
#include <stdio.h>

namespace utils
{
	void PRINT(const char* msg,...)
	{
		char str[256];
		char* args;

		args=(char*)&msg+sizeof(msg);
		vsprintf_s(str,msg,args);
		OutputDebugStringA(str);
	}
}
