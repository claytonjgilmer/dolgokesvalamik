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
#include "threading/taskmanager2.h"

#include "containers/string.h"
#include "containers/stringmap.h"

#include "file/file.h"
#include "file/filesystem.h"
#include "utils/timer.h"

#include "scripting/lua.h"
#include "render/renderobject3d.h"

#include "containers/poolalloc.h"

render::object3d* load_mmod(const char* i_filename);

struct proc
{
	void operator()(int* i_buf, unsigned i_num) const
	{
		for (unsigned n=0; n<i_num; ++n)
		{
			int sum=10;

			for (int m=0; m<100; ++m)
			{
				sum+=m;
			}

			i_buf[n]+=sum;
		}
	}
};

struct proc2
{
	unsigned m_bufsize;
	proc2(unsigned i_bufsize):m_bufsize(i_bufsize){}
	void operator()(int* i_buf, unsigned i_num) const
	{
		for (unsigned n=0; n<i_num; ++n)
		{
			threading::taskmanager::instance()->process_buffer(i_buf+n*m_bufsize,m_bufsize,10,proc());
		}
	}
};

class proc_task:public threading::task
{
public:
	int* m_buf;
	unsigned m_size;

	proc_task(int* i_buf, unsigned i_size):
	m_buf(i_buf),
	m_size(i_size)
	{
	}

	void run()
	{
		proc p;
		p(m_buf,m_size);
	}
};

void tasktest()
{
	threading::taskmanagerdesc tdesc; tdesc.m_threadnum=1;
	threading::taskmanager::create(&tdesc);
	physics::systemdesc pd;

	physics::system::create(&pd);

#define buf_size 15000
#define buf2_size 100

	int* buf=(int*)malloc(buf_size*buf2_size*sizeof(int));

	memset(buf,0,buf_size*buf2_size*sizeof(int));

	for (int n=0; n<buf2_size; ++n)
	{
		proc p;
		p(buf+n*buf_size,buf_size);
	}

	utils::timer t;

	t.reset();
	for (int n=0; n<buf2_size; ++n)
	{
		proc p;
		p(buf+n*buf_size,buf_size);
	}
	t.stop();
	unsigned tick2=t.get_tick();


/*
	t.reset();
	const unsigned tasknum=4;
	proc_task* task[tasknum];
	const unsigned egybe=buf_size/tasknum;
	for (int n=0; n<tasknum; ++n)
	{
		task[n]=new proc_task(buf+n*egybe,egybe);
	}
	threading::taskmanager::instance()->spawn_tasks((threading::task**)task,tasknum);
	t.stop();
	unsigned tick15=t.get_tick();
*/


	t.reset();
	threading::taskmanager::instance()->process_buffer(buf,buf2_size,1,proc2(buf_size));
	t.stop();
	unsigned tick=t.get_tick();




//	printf_s(" parallel:%d\n   serial:%d\nparallel2:%d\n",tick,tick2,tick15);
	printf_s(" parallel:%d\n   serial:%d\n",tick,tick2);

	free(buf);
	physics::system::release();
	threading::taskmanager::release();
}

#include "physics/physicssystem.h"

struct in
{
	math::mtx4x3 mtx;
	math::vec3 src;

	in()
	{
		mtx.identity();
		src.clear();
	}
};

struct probastruct2
{
	math::mtx4x3 mtx;
	math::vec3 src;
	math::vec3 dst;

	probastruct2* next;

	probastruct2()
	{
		mtx.identity();
		src.clear();
		next=0;
	}
};

in* g_p;
math::vec3* out;

void proba()
{

	utils::timer t;

#define szam 1000
#define szam2 10000

	unsigned tick1,tick2,tick3;
	
	g_p=new in[szam];
	out=new math::vec3[szam];
	t.reset();

	for (unsigned m=0; m<szam2;++m)
	for (unsigned n=0; n<szam;++n)
		g_p[n].mtx.transform(out[n],g_p[n].src);

	t.stop();
	tick1=t.get_tick();
	delete [] g_p;
	delete [] out;

	probastruct2 head;

	for (unsigned n=0; n<szam;++n)
	{
		probastruct2* uj=new probastruct2;
		uj->next=head.next;
		
		head.next=uj;
	}

	t.reset();

	for (unsigned n=0; n<szam2;++n)
	for (probastruct2* ptr=head.next; ptr; ptr=ptr->next)
		ptr->mtx.transform(ptr->dst,ptr->src);

	t.stop();
	tick2=t.get_tick();

	for (probastruct2* ptr=head.next; ptr;)
	{
		probastruct2* act=ptr; ptr=ptr->next;
		delete act;
	}

	printf_s("tomb :%d\nlista:%d\n",tick1,tick2);


}

int _cdecl main()
{
	proba();
	return 0;
	for (int n=0; n<1000; ++n)
	{
		printf_s("%d.\n",n+1);
		tasktest();
	}
}