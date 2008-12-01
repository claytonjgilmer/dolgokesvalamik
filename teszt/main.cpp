#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <MMSystem.h>

#include "math/sorting.h"
#include "math/vec3.h"
#include "math/math.h"
#include "containers/listallocator.h"
#include "physics/system/physicssystem.h"


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
#include <algorithm>
object3d* load_mmod(const char* i_filename);

struct proc
{
	void operator()(int* i_buf, unsigned i_num) const
	{
		for (unsigned n=0; n<i_num; ++n)
		{
			int sum=10;

			for (int m=0; m<100; ++m)
				sum+=m;

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
			taskmanager::ptr->process_buffer(i_buf+n*m_bufsize,m_bufsize,10,proc());
		}
	}
};

class proc_task:public task_t
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
	taskmanagerdesc tdesc; tdesc.m_threadnum=1;
	taskmanager::create(&tdesc);
	physicssystemdesc pd;

	physicssystem::create(&pd);

#define buf_size 15000
#define buf2_size 100

	int* buf=(int*)malloc(buf_size*buf2_size*sizeof(int));

	memset(buf,0,buf_size*buf2_size*sizeof(int));

	for (int n=0; n<buf2_size; ++n)
	{
		proc p;
		p(buf+n*buf_size,buf_size);
	}

	timer t;

	t.reset();
	for (int n=0; n<buf2_size; ++n)
	{
		proc p;
		p(buf+n*buf_size,buf_size);
	}
	t.stop();
	unsigned tick2=t.get_tick();


	t.reset();
	taskmanager::ptr->process_buffer(buf,buf2_size,1,proc2(buf_size));
	t.stop();
	unsigned tick=t.get_tick();




//	printf(" parallel:%d\n   serial:%d\nparallel2:%d\n",tick,tick2,tick15);
	printf(" parallel:%d\n   serial:%d\n",tick,tick2);

	free(buf);
	physicssystem::release();
	taskmanager::release();
}

struct in
{
	mtx4x3 mtx;
	vec3 src;

	in()
	{
		mtx.identity();
		src.clear();
	}
};

struct probastruct2
{
	mtx4x3 mtx;
	vec3 src;
	vec3 dst;

	probastruct2* next;

	probastruct2()
	{
		mtx.identity();
		src.clear();
		next=0;
	}
};

struct ps
{
	mtx4x3 mtx;
	vec3 src;
	vec3 dst;

	ps()
	{
		mtx.identity();
		src.clear();
	}
};

in* g_p;
vec3* out;

in** g_p2;

void proba()
{

	timer t;

#define szam 1000
#define szam2 1000

	unsigned tick1=0,tick2=0,tick3=0;

	{
		g_p2=new in*[1000];
		for (int n=0; n<1000;++n)
			g_p2[n]=(in*)malloc(random(1,5000)*sizeof(in));
	}

	for (unsigned m=0; m<szam2;++m)
	{
		{
			ps** p;
			ps* p2=new ps[szam];

			p=new ps*[szam];

			for (int n=0; n<szam; ++n)
//				p[n]=new ps;
				p[n]=p2+szam-1-n;

			t.reset();
			for (int n=0; n<szam; ++n)
				p[n]->mtx.transform(p[n]->dst,p[n]->src);

			t.stop();
			tick3+=t.get_tick();

//			for (int n=0; n<szam; ++n)
//				delete p[n];

			delete p2;

			delete [] p;
		}
		{
			probastruct2 head;

			for (unsigned n=0; n<szam;++n)
			{
				probastruct2* uj=new probastruct2;
				uj->next=head.next;

				head.next=uj;
			}

			t.reset();

			for (probastruct2* ptr=head.next; ptr; ptr=ptr->next)
				ptr->mtx.transform(ptr->dst,ptr->src);

			t.stop();
			tick2+=t.get_tick();

			for (probastruct2* ptr=head.next; ptr;)
			{
				probastruct2* act=ptr; ptr=ptr->next;
				delete act;
			}
		}
		{
			g_p=new in[szam];
			out=new vec3[szam];
			t.reset();

			for (unsigned n=0; n<szam;++n)
				g_p[n].mtx.transform(out[n],g_p[n].src);

			t.stop();
			tick1+=t.get_tick();
			delete [] g_p;
			delete [] out;
		}
	}

	printf("tomb :%d\nlista:%d\nlist2:%d\n",tick1,tick2,tick3);

	{
		for (int n=0; n<1000;++n)
			delete [] g_p2[n];

		delete [] g_p2;
	}


}

int _cdecl compareq(const void * a,const void * b)
{
	return (int)(*(float*)b-*(float*)a);
}

int compare(float a,float b)
{
	return a>b;
}

struct stdcomp
{
	bool operator()(float a,float b) const
	{
		return a<b;
	}
};

int compare2(float a,float b)
{
	return a<b;
}

unsigned get_radix_key(float f)
{
	return FloatToUnsigned(f);
}



//#include <algorithm>

void sorttest()
{
	const int elem_count=20000;
	float* array=new float[elem_count];
	float* arraystd=new float[elem_count];
	float* arrayquick=new float[elem_count];
	float* tmparray=new float[elem_count];

	for (int n=0; n<elem_count; ++n)
		tmparray[n]=0;
//		tmparray[n]=(float)random(0,100000);
//		tmparray[n]=float(elem_count-n);
//		tmparray[n]=float(n);

	memcpy(array,tmparray,elem_count*sizeof(float));

	timer t;

	t.reset();
	qsort(array,elem_count,sizeof(float),&compareq);
	t.stop();
	unsigned tickqqq=t.get_tick();
	memcpy(array,tmparray,elem_count*sizeof(float));

	t.reset();
	quicksortfloat(array,0,elem_count-1);
	t.stop();
	unsigned tickq3=t.get_tick();

	memcpy(array,tmparray,elem_count*sizeof(float));
	t.reset();
	std::sort(array,array+elem_count,&compare);
	t.stop();
	unsigned tickstd=t.get_tick();

	memcpy(arraystd,tmparray,elem_count*sizeof(float));
	t.reset();
	std::sort(arraystd,arraystd+elem_count,stdcomp());
	t.stop();
	unsigned tickstd2=t.get_tick();

	memcpy(array,tmparray,elem_count*sizeof(float));

	t.reset();
	q_sort<float ,&compare>(array,0,elem_count-1);
	t.stop();
	unsigned tickquick=t.get_tick();

	memcpy(arrayquick,tmparray,elem_count*sizeof(float));
	t.reset();
//	q_sort_rec<float ,&compare>(arrayquick,0,elem_count-1);
	qf(arrayquick,0,elem_count-1);
	t.stop();
	unsigned tickquickrec=t.get_tick();

	int eredmeny=memcmp(arraystd,arrayquick,elem_count*sizeof(float));

	printf("eredmeny:%d\n",eredmeny);

	memcpy(array,tmparray,elem_count*sizeof(float));
	t.reset();
	insertion_sort<float,&compare>(array,elem_count);
	t.stop();
	unsigned tickins=t.get_tick();

	memcpy(array,tmparray,elem_count*sizeof(float));
	t.reset();
	merge_sort<float,&compare>(array,elem_count);
	t.stop();
	unsigned tickmer=t.get_tick();

	memcpy(array,tmparray,elem_count*sizeof(float));
	t.reset();
	radixsort<float,&get_radix_key>(array,elem_count);
	t.stop();
	unsigned tickrad=t.get_tick();

	printf("qqq:%d\nstd:%d\nst2:%d\nqui:%d\nrec:%d\nq3 :%d\nins:%d\nmer:%d\nrad:%d\n",tickqqq,tickstd,tickstd2,tickquick,tickquickrec,tickq3,tickins,tickmer,tickrad);
}

uint32 sort_key;



//timer
typedef struct timer_t
{
	long long start;
	long long stop;
} timer_t;

long long timer_get_act_tick()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

void timer_start(timer_t* t)
{
	t->start=timer_get_act_tick();
}

void timer_stop(timer_t* t)
{
	t->stop=timer_get_act_tick();
}

unsigned timer_get_tick(timer_t* t)
{
	return (unsigned)(t->stop-t->start);
}


void sorttest2()
{
	timer_t t;
#define buff_size 20000
	//#define print_buf
	int buf[buff_size];
	int n;

	for (n=0; n<buff_size; ++n)
		//			buf[n]=buf_size-n;
//		buf[n]=0;
		buf[n]=rand();

#ifdef print_buf
	for (n=0; n<buff_size; ++n)
		printf("%d, ",buf[n]);

	printf("\n");
#endif

	timer_start(&t);
	std::sort(buf,buf+buff_size);
	timer_stop(&t);

	printf("%d elem sortolasa:%d\n",buff_size,timer_get_tick(&t));

#ifdef print_buf
	for (n=0; n<buff_size; ++n)
		printf("%d, ",buf[n]);

	printf("\n");
#endif
}

vec3 x,y;
typedef int* tipus;

class vvvvv
{
public:
	float x,y,z;

//	virtual ~vvvvv(){}
};

void fib(int n,int& f)
{
    if (n==1) f=1;
    else if (n==2) f=2;
    else
    {
        int f1,f2;
        fib(n-1,f1);
        fib(n-2,f2);
        f=f1+f2;
    }
}

int _cdecl main()
{
    for (int n=1; n<=50; ++n)
    {
        int f;
        fib(n,f);
        printf("fibonacci(%d)=%d\n",n,f);
    }
    return 0;
	tipus valami1=tipus();
	vvvvv valami2=vvvvv();
//	valami2=
//	valami2.x=1; valami2.y=2; valami2.z=3;

//	new (&valami2) vvvvv ();

	printf("pointer:%d\n",valami1);
	printf("vec3 :%f %f %f\n",valami2.x,valami2.y,valami2.z);
	return 0;
	vec3 a1,a2; a1.set(1,2,3),a2.set(4,5,6);
	x=a1+a2;
	y=x;
	x=add(a1,a2);
	sorttest2();
	return 0;
	proba();
	return 0;
	for (int n=0; n<1000; ++n)
	{
		printf("%d.\n",n+1);
		tasktest();
	}
}
