#include "math/vec3.h"
#include "math/math.h"
#include "math/ssemtx4x3.h"
#include "containers/listallocator.h"
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <MMSystem.h>
#include "physics/physicssystem.h"

#include "threading/thread.h"
#include "threading/taskmanager.h"

#include "containers/string.h"
#include "containers/stringmap.h"

#include "file/file.h"
#include "file/filesystem.h"
#include "utils/timer.h"

#include "scripting/lua.h"
#include "render/renderobject3d.h"

#include "containers/poolalloc.h"

render::object3d* load_mmod(const char* i_filename);

int _cdecl main()
{
	int* a=new int[113];
	load_mmod("test.mmod");
	return 0;
#if 0
	utils::timer t;

	const unsigned iternum=100000;
	const unsigned maxval=1000000;

	unsigned* val=new unsigned[iternum+1];
	for (unsigned n=0; n<=iternum;++n)
	{
		unsigned short r1=rand();
		unsigned short r2=rand();

		val[n]=(((unsigned)r1) << 16 ) + r2;
		val[n]%=maxval;
		++val[n];
	}


	t.reset();
	for (unsigned n=0; n<iternum; ++n)
	{
		math::gcd(val[n],val[n+1]);
	}
	t.stop();
	printf("1 time:%d\n",t.get_tick());

	t.reset();
	for (unsigned n=0; n<iternum; ++n)
	{
		math::gcd2(val[n],val[n+1]);
	}
	t.stop();
	printf("1 time:%d\n",t.get_tick());

	delete [] val;
#endif
	scripting::lua Lua;
	Lua.InitState((scripting::lua::eLuaLib)(scripting::lua::LL_BASE | scripting::lua::LL_MATH));//| scripting::lua::LL_IO| scripting::lua::LL_STRING));

/*
	scripting::lua::Variable inittable=Lua.GetGlobalTable().GetVariable("InitParams");

	if (inittable.IsTable())
	int width=inittable.GetVariable("Width").GetInt(640);
	else if (inittable.IsNil())
		strlen("");
*/

	for (int n=0; n<4; ++n)
	{
		Lua.DoFile("faszom.lua");
		system("PAUSE");
//		while (!_kbhit());
//		while (_kbhit());
	}
}