#ifndef _math_h_
#define _math_h_

#include <math.h>
#include <stdlib.h>
#include <float.h>
#include "..\utils\misc.h"
#include "utils\assert.h"

typedef unsigned __int64 uint64;
typedef unsigned uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef __int64 int64;
typedef int int32;
typedef short int16;
typedef char int8;
typedef double f64;

#ifdef _DEBUG
MLINLINE void check_float(float value)
{
	int s, e;
	unsigned long src;
	long f;
	unsigned int *srcptr = (unsigned int *)&value;

	src = *srcptr;

	s = (src & 0x80000000UL) >> 31;
	e = (src & 0x7F800000UL) >> 23;
	f = (src & 0x007FFFFFUL);

	if (value==9872)
	{
		assertion(0);
	}
	if (e == 255 && f != 0)
	{
		assertion(0);
	}
	else if (e == 255 && f == 0 && s == 1)
	{
		assertion(0);
	}
	else if (e == 255 && f == 0 && s == 0)
	{
		assertion(0);
	}
}
#else
#define check_float(_A_)
#endif


#if 0//def _DEBUG
struct f32
{
	float f;

	f32()
	{
		f=9872;
	}
	f32(const f32& fl)
	{
		check_float(fl.f);
		f=fl.f;
	}
	f32(const float& fl)
	{
		check_float(fl);
		f=fl;
	}

	f32& operator=(const f32& fl)
	{
		check_float(fl.f);
		f=fl.f;
		return *this;
//		return f;
	}
	void operator/=(const f32& fl)
	{
		check_float(f);
		check_float(fl.f);
		f/=fl.f;
	}
	void operator*=(const f32& fl)
	{
		check_float(f);
		check_float(fl.f);
		f*=fl.f;
	}
	void operator+=(const f32& fl)
	{
		check_float(f);
		check_float(fl.f);
		f+=fl.f;
		//		return f;
	}
	void operator-=(const f32& fl)
	{
		check_float(f);
		check_float(fl.f);
		f=fl.f;
	}

	operator float() const
	{
		return f;
	}

/*
	float operator+(const f32& fl) const
	{
		check_float(fl.f);
		return f+fl.f;
	}
	float operator-(const f32& fl) const
	{
		check_float(fl.f);
		return f-fl.f;
	}
	float operator*(const f32&  fl) const
	{
		check_float(fl.f);
		return f*fl.f;
	}
	float operator/(const f32& fl) const
	{
		check_float(fl.f);
		return f/fl.f;
	}

	void operator+=(const f32& fl)
	{
		check_float(fl.f);
		f+=fl.f;
	}
	void operator-=(const f32& fl)
	{
		check_float(fl.f);
		f-=fl.f;
	}
	void operator*=(const f32& fl)
	{
		check_float(fl.f);
		f*=fl.f;
	}
	void operator/=(const f32& fl)
	{
		check_float(fl.f);
		f/=fl.f;
	}
*/
};
#else
typedef float f32;
#endif

/*
	union floattounsigned
	{
		float f;
		unsigned i;

		floattounsigned(f32 i_value)
		{
			f=i_value;
			i^=((~(i >> 31))+1) | 0x80000000;
		}

		void operator =(f32 i_value)
		{
			f=i_value;
			i^=((~(i >> 31))+1) | 0x80000000;
		}

		floattounsigned()
		{
		}

		floattounsigned(unsigned i_value)
		{
			i=i_value;
		}

		void operator=(unsigned i_value)
		{
			i=i_value;
		}

		operator const unsigned&() const
		{
			return i;
		}
	};
*/


	MLINLINE f32 pi()
	{
		return 3.1415926536f;
	}

	MLINLINE f32 twopi()
	{
		return 2.0f*pi();
	}

	MLINLINE f32 pihalf()
	{
		return 0.5f*pi();
	}
/*
	MLINLINE f32 sin(f32 i_value)
	{
		return sinf(i_value);
	}

	MLINLINE f32 cos(f32 i_value)
	{
		return cosf(i_value);
	}

	MLINLINE f32 abs(f32 i_value)
	{
		return fabsf(i_value);
	}

	MLINLINE f32 asin(f32 i_value)
	{
		return asinf(i_value);
	}

	MLINLINE f32 acos(f32 i_value)
	{
		return acosf(i_value);
	}

	MLINLINE f32 atan2(f32 i_value1,f32 i_value2)
	{
		return atan2f(i_value1,i_value2);
	}

	MLINLINE f32 sqrt(f32 i_value)
	{
		return sqrtf(i_value);
	}

	MLINLINE f32 pow(f32 i_value, f32 i_power)
	{
		return powf(i_value,i_power);
	}
*/

	template<typename t> t sqr(t i_value)
	{
		return i_value*i_value;
	}

	MLINLINE f32 radtodegree(f32 i_radian)
	{
		return i_radian*(180.0f/pi());
	}

	MLINLINE f32 degreetorad(f32 i_degree)
	{
		return i_degree*(pi()/180.0f);
	}

	template<typename T> MLINLINE T min(const T& i_value1, const T& i_value2)
	{
		return i_value1<i_value2 ? i_value1 : i_value2;
	}

	template<typename t> MLINLINE t max(t i_value1, t i_value2)
	{
		return i_value1>i_value2 ? i_value1 : i_value2;
	}

	template<typename t> MLINLINE t clamp(t i_src, t i_minvalue, t i_maxvalue)
	{
		if (i_src<i_minvalue)
			return i_minvalue;

		if (i_src>i_maxvalue)
			return i_maxvalue;

		return i_src;

	}
	MLINLINE f32 clamp(f32 i_src, f32 i_minvalue, f32 i_maxvalue)
	{
		if (i_src<i_minvalue)
			return i_minvalue;

		if (i_src>i_maxvalue)
			return i_maxvalue;

		return i_src;

	}

	MLINLINE f32 sign(f32 i_value)
	{
		return i_value>0.0f ? 1.0f : -1.0f;
	}

	MLINLINE double sign(double i_value)
	{
		return i_value>0.0 ? 1.0 : -1.0;
	}

/*
	MLINLINE void swap(int& io_value1, int& io_value2)
	{
		io_value1^=io_value2;
		io_value2^=io_value1;
		io_value1^=io_value2;
	}

	MLINLINE void swap(f32& io_value1, f32& io_value2)
	{
		swap((int&)io_value1,(int&)io_value2);
	}
*/
	template<typename t>
	MLINLINE void swap(t& io_value1, t& io_value2)
	{
		t tmp=io_value1;
		io_value1=io_value2;
		io_value2=tmp;
	}

/*
	template<typename t>
	MLINLINE void sort(t& id0, t& id1)
	{
		if(id0>id1)	swap(id0, id1);
	}
*/

    template<typename T>
    MLINLINE T* min_elem(T* first, T* last)
    {
        T* m=first;
        T* ptr;
        for(ptr=first,++ptr; ptr!=last; ++ptr)
            if (*ptr<*m)
                m=ptr;

        return m;
    }

	template<typename T>
	MLINLINE T* max_elem(T* first, T* last)
	{
		T* m=first;
		T* ptr;
		for(ptr=first,++ptr; ptr!=last; ++ptr)
			if (*ptr>*m)
				m=ptr;

		return m;
	}


	MLINLINE unsigned nextpoweroftwo(unsigned i_value)
	{
		i_value|= (i_value >> 1);
		i_value|= (i_value >> 2);
		i_value|= (i_value >> 4);
		i_value|= (i_value >> 8);
		i_value|= (i_value >> 16);
		return i_value+1;
	}

	MLINLINE int ispoweroftwo(unsigned i_value)
	{
		return !(i_value & (i_value-1));
	}

	MLINLINE int nextnumbermodulo3(int i_actnumber)
	{
		return (1 << i_actnumber) & 3;
	}

	template<typename t> t interpolate(const t& x, const t& p1, const t& p2, const t& v1, const t& v2)
	{
		return ((v2-v1)/(p2-p1))*(x-p1)+v1;
	}

	template<typename t> t interpolate(f32 x, const t& v1, const t& v2)
	{
		return (v2-v1)*x+v1;
	}
/*
	MLINLINE f32 inv_sqrt(f32 i_val)
	{
		union {f32 f;int i;} val;
		val.f=i_val;
		f32 xhalf=0.5f*i_val;

//		val.i=0x5f3fba86-(val.i>>1);
//		val.i=0x5f3fba86-(val.i>>1);
		val.i=0x5f375a86-(val.i>>1);
//		val.i=0x5f3759df-(val.i>>1);
		return val.f*(1.5f-xhalf*val.f*val.f);
	}

	MLINLINE f32 fast_sqrt(f32 i_val)
	{
		union {f32 f;int i;} val;
		val.f=i_val;
		f32 xhalf=0.5f*i_val;

		//		val.i=0x5f3fba86-(val.i>>1);
		//		val.i=0x5f3fba86-(val.i>>1);
		val.i=0x5f375a86-(val.i>>1);
		//		val.i=0x5f3759df-(val.i>>1);
		return i_val*val.f*(1.5f-xhalf*val.f*val.f);
	}
	template <int n> MLINLINE int logn(){return 1+logn<(n >> 1)>();}
	template<> MLINLINE int logn<1>(){return 0;}
*/
	MLINLINE unsigned greatest_common_divisor(unsigned a1, unsigned a2)
	{
		while (a1!=a2)
		{
			if (a1<a2)
				a2-=a1;
			else
				a1-=a2;
		}

		return a1;
	}

	MLINLINE unsigned greatest_common_divisor2(unsigned a1, unsigned a2)
	{
		while (a1 && a2)
		{
			if (a1<a2)
				a2%=a1;
			else
				a1%=a2;
		}

		return a1 ? a1 : a2;
	}

	MLINLINE unsigned least_common_multiple(unsigned a1, unsigned a2)
	{
		return a1*a2/greatest_common_divisor(a1,a2);
	}

	MLINLINE unsigned least_common_multiple2(unsigned a1, unsigned a2)
	{
		return a1*a2/greatest_common_divisor2(a1,a2);
	}

	MLINLINE unsigned least_common_multiple3(unsigned a1, unsigned a2)
	{
		unsigned k1=a1,k2=a2;

		while (k1!=k2)
			if (k1<k2) k1+=a1; else k2+=a2;

		return k1;
	}


	MLINLINE f32 random(f32 i_min, f32 i_max)
	{
		return (f32)rand()/(f32)RAND_MAX*(i_max-i_min)+i_min;
	}

	MLINLINE int  random(int i_min, int i_max)
	{
		return (rand() % (i_max+1-i_min))+i_min;
//		return (rand())*(i_max-i_min)/RAND_MAX+i_min;
	}

	MLINLINE f32 med3(f32 a1,f32 a2,f32 a3)
	{
		if (a1<a2)
		{
			if (a2<a3) return a2; //a1-a2-a3
			if (a1<a3) return a3;//a1-a3-a2

			return a1; //a3-a1-a2;
		}
		else
		{
			if (a1<a3) return a1; //a2-a1-a3;
			if (a2<a3) return a3; //a2-a3-a1;
			return a2; //a3-a2-a1
		}
	}


#endif// _math_h_
