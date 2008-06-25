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

template<int n> int fact(){return n*fact<n-1>();}
template<> int fact<1>(){return 1;}

math::mtx4x3 gmtx(math::mtx4x3::identitymtx());
math::ssemtx4x3 ssegmtx(math::ssemtx4x3::identitymtx());

unsigned WINAPI threadrun(void* i_param)
{
	threading::thread* t=(threading::thread*)i_param;

	for (int n=0; n<100; ++n)
	{
		printf_s("thread%d: %d\n",t->getid(),n);
	}

	return 0;
}

class procclass
{
public:
	procclass(int& i_sum):sum(i_sum){}
	void operator()(int* i_buf, unsigned i_num) const
	{
		int l=0;
		for (unsigned n=0; n<i_num; ++n)
		{
			//				l+=i_buf[n];
			l+=(int)math::sqrt((float)(i_buf[n]*i_buf[n]*i_buf[n]));
		}

		sum+=l;
	}

	int& sum;
};

class mapelem
{
public:
	mapelem(ctr::string i_name):Name(i_name){}
	ctr::string Name;

	mapelem* Next;
};

#define _ME_(_NAME_) mapelem _NAME_(#_NAME_)

int _cdecl main()
{
	ctr::stringmap<mapelem,128> strmap;

	unsigned key;

	_ME_(a000);
	_ME_(b000);
	_ME_(c000);
	_ME_(d000);
	_ME_(e000);

	key=strmap.add_data(&a000);
	key=strmap.add_data(&b000);
	key=strmap.add_data(&c000);
	key=strmap.add_data(&d000);
	key=strmap.add_data(&e000);
	mapelem* T;
	T=strmap.get_data("egy");
	T=strmap.get_data("ketto");
	T=strmap.get_data("harom");
	T=strmap.get_data("negy");
/*
	threading::thread th1("1"),th2("2");

	th1.start(&threadrun,&th1);
	th2.start(&threadrun,&th2);
	th1.join();
	th2.join();
*/


	threading::taskmanager tm(2);
#if 0
	class spectask:public threading::task
	{
	public:
		virtual void run()
		{
			printf_s("%d: ez aztan a valami!\n", get_id());
		}
	};

	unsigned depi=-1;
	ctr::fixedvector<unsigned,10> dv;

	for (int n=0; n<50;++n)
	{
		spectask * tk1=new(tm.get_allocator()) spectask;
		depi=tm.spawn_task(tk1,-1,dv);
		dv.clear();
		dv.push_back(depi);
	}
#endif
	const int bufsize=100999;//000;
	static int buf[bufsize];

	for (int n=0; n<bufsize;++n)
	{
		buf[n]=1;
	}

	static int sum=0;

	{
		LARGE_INTEGER starttime;
		::QueryPerformanceCounter(&starttime);
		sum=0;

		int s=0;

		for (int n=0; n<bufsize;++n)
		{
			s+=(int)math::sqrt((float)(buf[n]*buf[n]*buf[n]));
		}

		sum=s;

		LARGE_INTEGER endtime;
		::QueryPerformanceCounter(&endtime);

		LARGE_INTEGER freq; ::QueryPerformanceFrequency(&freq);

		printf_s("serialtime: %f\n",(endtime.QuadPart-starttime.QuadPart)/(double)freq.QuadPart);
	}

	sum=0;


	LARGE_INTEGER starttime;
	::QueryPerformanceCounter(&starttime);
	long long time=starttime.QuadPart;

	;
	unsigned id=tm.process_buffer(buf,bufsize,10,procclass(sum));

	class printsum:public threading::task
	{
	public:
		void run()
		{
			LARGE_INTEGER starttime;
			::QueryPerformanceCounter(&starttime);
			LARGE_INTEGER freq; ::QueryPerformanceFrequency(&freq);

			double sec=(starttime.QuadPart-time)/(double)freq.QuadPart;

			printf_s("sum:%d\n",sum);
			printf_s("time:%f\n",sec);
		}

		int& sum;
		long long time;

		printsum(int& i_sum, long long i_time):sum(i_sum), time(i_time){}
	};

	tm.spawn_task(new (tm.get_allocator()) printsum(sum,time),-1,id);









	tm.flush();
//	int x;

//	std::cin >> x;
//	return;
	int a=fact<5>();

	printf_s("%d!=%d\n",5,a);

	const int b=15;
	printf_s("log(%d):%d\n",b,math::logn<b>());
	math::vec3 v1,v2,v3;

	v1.set(1,2,3);

	ctr::listallocator<int,128> la;

	la.allocate();
	//	la.m_pointer.push_back(1);

	physics::systemdesc sd;
	physics::system* s=physics::system::createsystem(sd);

	physics::bodydesc bd;
	physics::body* b1=s->createbody(bd);
	bd.m_vel+=math::vec3(1,2,3);
	bd.m_rotvel=math::vec3(3,4,5);
	bd.m_pos.trans()=math::vec3(4,5,6);
	physics::body* b2=s->createbody(bd);

	s->releasebody(b1);

	s->releasebody(b2);

	s->release();

	const int itnum=10000000;

	{
		math::mtx4x3 mtx(gmtx);//; mtx.identity();
		mtx.xaxis.set(1.0000001f,0.0000001f,0.0000001f);
		mtx.yaxis.set(0.0000001f,1.0000001f,0.0000001f);
		mtx.zaxis.set(0.0000001f,0.0000001f,1.0000001f);
		mtx.t.set(0.00001f,0.00001f,0.00001f);
		math::vec3 vec; vec.set(5,2,3);
		math::vec3 vec2;

		unsigned start=timeGetTime();

		for (int n=0; n<itnum; ++n)
		{
			mtx.transform2(vec2,vec);
			mtx.transform2(vec,vec2);
		}

		unsigned end=timeGetTime();

		printf_s("time:%d\n",end-start);
		printf_s("vec:%f %f %f\n",vec.x,vec.y,vec.z);
	}

	{
//		math::mtx4x3 mtx; mtx.identity();
		math::mtx4x3 mtx(gmtx);
		mtx.xaxis.set(1.0000001f,0.0000001f,0.0000001f);
		mtx.yaxis.set(0.0000001f,1.0000001f,0.0000001f);
		mtx.zaxis.set(0.0000001f,0.0000001f,1.0000001f);
		mtx.t.set(0.00001f,0.00001f,0.00001f);
		math::vec3 vec; vec.set(5,2,3);
		math::vec3 vec2;

		unsigned start=timeGetTime();

		for (int n=0; n<itnum; ++n)
		{
			mtx.transform(vec2,vec);
			mtx.transform(vec,vec2);
		}

		unsigned end=timeGetTime();

		printf_s("time:%d\n",end-start);
		printf_s("vec:%f %f %f\n",vec.x,vec.y,vec.z);
	}

	{
		math::ssemtx4x3 mtx(ssegmtx);
		mtx.xaxis.set(1.0000001f,0.0000001f,0.0000001f);
		mtx.yaxis.set(0.0000001f,1.0000001f,0.0000001f);
		mtx.zaxis.set(0.0000001f,0.0000001f,1.0000001f);
		mtx.trans.set(.00001f,.00001f,.00001f);
		math::ssevec3 vec; vec.set(5,2,3);
		math::ssevec3 vec2;

		unsigned start=timeGetTime();

		for (int n=0; n<itnum; ++n)
		{
			vec2=mtx.transform(vec);
			vec=mtx.transform(vec2);
//			mtx.transform(vec2,vec);
//			mtx.transform(vec,vec2);
		}

		unsigned end=timeGetTime();

		printf_s("time:%d\n",end-start);
		printf_s("vec:%f %f %f\n",vec[0],vec[1],vec[2]);
	}

	while (!_kbhit());
}