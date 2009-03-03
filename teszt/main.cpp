#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <MMSystem.h>

#include "math/sorting.h"
#include "math/vec3.h"
#include "math/math.h"
#include "containers/listallocator.h"
#include "physics/system/physicssystem.h"

#include "math\geometry\convexhullgeneration.h"


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

struct proc_task:task_t
{
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

	timer_t t;

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

	timer_t t;

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
	return (int)(*(f32*)b-*(f32*)a);
}

int compare(f32 a,f32 b)
{
	return a>b;
}

struct stdcomp
{
	bool operator()(f32 a,f32 b) const
	{
		return a<b;
	}
};

int compare2(f32 a,f32 b)
{
	return a<b;
}

unsigned get_radix_key(f32 f)
{
	return FloatToUnsigned(f);
}



//#include <algorithm>

void sorttest()
{
	const int elem_count=20000;
	f32* array=new f32[elem_count];
	f32* arraystd=new f32[elem_count];
	f32* arrayquick=new f32[elem_count];
	f32* tmparray=new f32[elem_count];

	for (int n=0; n<elem_count; ++n)
		tmparray[n]=0;
//		tmparray[n]=(f32)random(0,100000);
//		tmparray[n]=f32(elem_count-n);
//		tmparray[n]=f32(n);

	memcpy(array,tmparray,elem_count*sizeof(f32));

	timer_t t;

	t.reset();
	qsort(array,elem_count,sizeof(f32),&compareq);
	t.stop();
	unsigned tickqqq=t.get_tick();
	memcpy(array,tmparray,elem_count*sizeof(f32));

	t.reset();
	quicksortfloat(array,0,elem_count-1);
	t.stop();
	unsigned tickq3=t.get_tick();

	memcpy(array,tmparray,elem_count*sizeof(f32));
	t.reset();
	std::sort(array,array+elem_count,&compare);
	t.stop();
	unsigned tickstd=t.get_tick();

	memcpy(arraystd,tmparray,elem_count*sizeof(f32));
	t.reset();
	std::sort(arraystd,arraystd+elem_count,stdcomp());
	t.stop();
	unsigned tickstd2=t.get_tick();

	memcpy(array,tmparray,elem_count*sizeof(f32));

	t.reset();
	q_sort<f32 ,&compare>(array,0,elem_count-1);
	t.stop();
	unsigned tickquick=t.get_tick();

	memcpy(arrayquick,tmparray,elem_count*sizeof(f32));
	t.reset();
//	q_sort_rec<f32 ,&compare>(arrayquick,0,elem_count-1);
	qf(arrayquick,0,elem_count-1);
	t.stop();
	unsigned tickquickrec=t.get_tick();

	int eredmeny=memcmp(arraystd,arrayquick,elem_count*sizeof(f32));

	printf("eredmeny:%d\n",eredmeny);

	memcpy(array,tmparray,elem_count*sizeof(f32));
	t.reset();
	insertion_sort<f32,&compare>(array,elem_count);
	t.stop();
	unsigned tickins=t.get_tick();

	memcpy(array,tmparray,elem_count*sizeof(f32));
	t.reset();
	merge_sort<f32,&compare>(array,elem_count);
	t.stop();
	unsigned tickmer=t.get_tick();

	memcpy(array,tmparray,elem_count*sizeof(f32));
	t.reset();
	radixsort<f32,&get_radix_key>(array,elem_count);
	t.stop();
	unsigned tickrad=t.get_tick();

	printf("qqq:%d\nstd:%d\nst2:%d\nqui:%d\nrec:%d\nq3 :%d\nins:%d\nmer:%d\nrad:%d\n",tickqqq,tickstd,tickstd2,tickquick,tickquickrec,tickq3,tickins,tickmer,tickrad);
}

uint32 sort_key;



//timer
void timer_start(timer_t* t)
{
	t->reset();
}

void timer_stop(timer_t* t)
{
	t->stop();
}

unsigned timer_get_tick(timer_t* t)
{
	return t->get_tick();
}


template<int coord=0>
struct vec_sort_t_2
{
	bool operator()(const vec3& v1, const vec3& v2) const
    {
        return v1[coord]<v2[coord];
    }
    static bool less(const vec3& v1, const vec3& v2)
    {
        return v1[coord]<v2[coord];
    }
    static bool eq(const vec3& v1, const vec3& v2)
    {
        return v1[coord]==v2[coord];
    }
};
namespace lofasz
{

void sorttest2()
{
	timer_t t;
#define buff_size 4000
	//#define print_buf
	vec3 buf[buff_size];
	vec3 buf2[buff_size];
	vec3 buf3[buff_size];
	int n;

	convex_hull_desc hd;
	hd.face_thickness=.001f;
	vector<vec3>& b=hd.vertex_array;
//	b.resize(buff_size);

	for (n=0; n<buff_size; ++n)
		for (int m=0; m<3; ++m)
		{
//		buf[n][m]=buff_size-n;
//		buf[n][m]=0;
		buf[n][m]=random(-10.0f, 10.0f);

		buf2[n][m]=buf[n][m];
		buf3[n][m]=buf[n][m];
//		b[n][m]=buf[n][m];
		}

#ifdef print_buf
	for (n=0; n<buff_size; ++n)
		printf("%d, ",buf[n]);

	printf("\n");
#endif
	b.resize(9);

	b[0].set(-1,0,-1);
	b[1].set(-1,0,1);
	b[2].set(-1,1,-1);
	b[3].set(-1,1,1);
	b[4].set(1,0,-1);
	b[5].set(1,0,1);
	b[6].set(1,1,-1);
	b[7].set(1,1,1);
	b[8].set(1,1.5f,1);
	timer_start(&t);
	convex_hull_t h=generate_convex_hull(hd);
	timer_stop(&t);
	printf("%d elem convexhull generalasa:%d\n",buff_size,timer_get_tick(&t));
/*
	vector<vec3> b2;
	simplify_vertex_array(b2,b);

	for (n=0; n<b.size(); ++n)
		printf("%d: %.2f, %.2f, %.2f\n", n, b[n].x,b[n].z,b[n].y);
	for (n=0; n<b2.size(); ++n)
		printf("%d: %.2f, %.2f, %.2f\n", n, b2[n].x,b2[n].z,b2[n].y);
*/


	timer_start(&t);
	std::sort(buf,buf+buff_size,vec_sort_t_2<0>());
	timer_stop(&t);
	printf("%d elem sortolasa:%d\n",buff_size,timer_get_tick(&t));


	timer_start(&t);
	introsort<vec3,vec_sort_t_2<0> >i; i(buf2,buff_size);
	timer_stop(&t);

	printf("%d elem sortolasa:%d\n",buff_size,timer_get_tick(&t));

	timer_start(&t);
	intro_sort(buf3,buff_size,vec_sort_t_2<0>());
	timer_stop(&t);

	printf("%d elem sortolasa:%d\n",buff_size,timer_get_tick(&t));
#ifdef print_buf
	for (n=0; n<buff_size; ++n)
		printf("%d, ",buf[n]);

	printf("\n");
#endif
}
}

allocator_list_elem* allocator_list_elem::s_allocator_pool_head=NULL;

void allocator_gc(allocator_list_elem* ptr)
{
	while (ptr)
	{
		ptr->garbage_collection();
		ptr=ptr->next;
	}
}

#include "containers/listallocator.h"

int szam_beker(const char* szoveg)
{
	printf(szoveg);
	char szamstr[128];

	gets(szamstr);

	return atoi(szamstr);
}


#include "containers/staticset.h"

int _cdecl main()
{
#define bufsize 16
#define setcount 4
	static_set<int,bufsize,setcount> set;

	for (int n=0; n<16; ++n)
	{
		int elem=random(0,10);
		int set_index=random(0,1)*2;

		printf ("%d elem a %d halmazba\n",elem,set_index);
		set.push(elem,set_index);

		for (int m=0; m<setcount; ++m)
		{
			printf("%d halmaz: ",m);

			if (set.empty(m))
			{
				printf("ures\n");
			}
			else
			{
				for (int k=set.set_bottom[m]; k!=((set.set_top[m]+1) & (bufsize-1)); k=((k+1) & (bufsize-1)))
				{
					printf("%d  ",set.buffer[k]);
				}

				printf("\n");
			}
		}
	}







//	lofasz::sorttest2();

/*
	pool_allocator<32,64> pa[4];

	vector<void*> ptr[4];


	for (int n=0; n<1000; ++n)
	{
		for (int k=0; k<4; ++k)
		{
			if (random(0,10)<5)
				ptr[k].push_back(pa[k].alloc());
		}
	}

	for (int k=0; k<4; ++k)
	{
		for (int n=0; n<ptr[k].size(); ++n)
			pa[k].free(ptr[k][n]);
	}

	allocator_gc(allocator_list_elem::s_allocator_pool_head);
*/

	int hoadd[]={0,3,3,6,1,4,6,2,5,0,3,5};

	char* napstr[]={"vasarnap","hetfo","kedd","szerda","csutortok","pentek","szombat"};

	while (1)
	{
		int ev=szam_beker("evszam:");
		int ho=szam_beker("honap:");
		int nap=szam_beker("nap:");

		int ret=(ev-1900)+(ev-1900)/4+hoadd[(ho+11) % 12]+nap;
		if (!(ev % 4) && ho<3) --ret;
		ret%=7;
		printf("%d.%02d.%02d. %s.\n\n",ev,ho,nap,napstr[ret]);
	}
	return 0;


	while (1)
	{
		int sz=szam_beker("irj be egy egynel nagyobb pozitiv szamot:");

		if (sz==-1)
			return 0;

		if (sz<=1)
		{
			printf("buzi-e vagy???\n");
		}

		printf ("%d=",sz);

		int oszto=2;
//		int gyok=(int)sqrtf((f32)sz);
		while (oszto<=(int)sqrtf((f32)sz))
		{
			if (!(sz%oszto))
			{
				printf("%d*",oszto);
				sz/=oszto;
			}
			else
				oszto+=1;
		}

		printf("%d\n",sz);
	}
	return 0;
	printf("faszkalap");
	return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;return 0;
}
