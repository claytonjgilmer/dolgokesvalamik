#ifndef _math_h_
#define _math_h_

#include <math.h>
#include <stdlib.h>
#include <float.h>
#include "..\utils\misc.h"

typedef unsigned __int64 uint64;
typedef unsigned uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef __int64 int64;
typedef int int32;
typedef short int16;
typedef char int8;


	union floattounsigned
	{
		float f;
		unsigned i;

		floattounsigned(float i_value)
		{
			f=i_value;
			i^=((~(i >> 31))+1) | 0x80000000;
		}

		void operator =(float i_value)
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



	MLINLINE float pi()
	{
		return 3.1415926536f;
	}

	MLINLINE float twopi()
	{
		return 2.0f*pi();
	}

	MLINLINE float pihalf()
	{
		return 0.5f*pi();
	}
/*
	MLINLINE float sin(float i_value)
	{
		return sinf(i_value);
	}

	MLINLINE float cos(float i_value)
	{
		return cosf(i_value);
	}

	MLINLINE float abs(float i_value)
	{
		return fabsf(i_value);
	}

	MLINLINE float asin(float i_value)
	{
		return asinf(i_value);
	}

	MLINLINE float acos(float i_value)
	{
		return acosf(i_value);
	}

	MLINLINE float atan2(float i_value1,float i_value2)
	{
		return atan2f(i_value1,i_value2);
	}

	MLINLINE float sqrt(float i_value)
	{
		return sqrtf(i_value);
	}

	MLINLINE float pow(float i_value, float i_power)
	{
		return powf(i_value,i_power);
	}
*/

	template<typename t> t sqr(t i_value)
	{
		return i_value*i_value;
	}

	MLINLINE float radtodegree(float i_radian)
	{
		return i_radian*(180.0f/pi());
	}

	MLINLINE float degreetorad(float i_degree)
	{
		return i_degree*(pi()/180.0f);
	}

	template<typename T> MLINLINE T min(const T& i_value1, const T& i_value2)
	{
		return i_value1<i_value2 ? i_value1 : i_value2;
	}

	template<class t> MLINLINE t max(t i_value1, t i_value2)
	{
		return i_value1>i_value2 ? i_value1 : i_value2;
	}

	template<class t> MLINLINE t clamp(t i_src, t i_minvalue, t i_maxvalue)
	{
		if (i_src<i_minvalue)
			return i_minvalue;

		if (i_src>i_maxvalue)
			return i_maxvalue;

		return i_src;

	}

	MLINLINE float sign(float i_value)
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

	MLINLINE void swap(float& io_value1, float& io_value2)
	{
		swap((int&)io_value1,(int&)io_value2);
	}
*/
	template<class t>
	MLINLINE void swap(t& io_value1, t& io_value2)
	{
		t tmp=io_value1;
		io_value1=io_value2;
		io_value2=tmp;
	}

	template<class t>
	MLINLINE void sort(t& id0, t& id1)
	{
		if(id0>id1)	swap(id0, id1);
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

	template<class t> t interpolate(const t& x, const t& p1, const t& p2, const t& v1, const t& v2)
	{
		return ((v2-v1)/(p2-p1))*(x-p1)+v1;
	}

	template<class t> t interpolate(float x, const t& v1, const t& v2)
	{
		return (v2-v1)*x+v1;
	}

	MLINLINE float inv_sqrt(float i_val)
	{
		union {float f;int i;} val;
		val.f=i_val;
		float xhalf=0.5f*i_val;

//		val.i=0x5f3fba86-(val.i>>1);
//		val.i=0x5f3fba86-(val.i>>1);
		val.i=0x5f375a86-(val.i>>1);
//		val.i=0x5f3759df-(val.i>>1);
		return val.f*(1.5f-xhalf*val.f*val.f);
	}

	MLINLINE float fast_sqrt(float i_val)
	{
		union {float f;int i;} val;
		val.f=i_val;
		float xhalf=0.5f*i_val;

		//		val.i=0x5f3fba86-(val.i>>1);
		//		val.i=0x5f3fba86-(val.i>>1);
		val.i=0x5f375a86-(val.i>>1);
		//		val.i=0x5f3759df-(val.i>>1);
		return i_val*val.f*(1.5f-xhalf*val.f*val.f);
	}
	template <int n> MLINLINE int logn(){return 1+logn<(n >> 1)>();}
	template<> MLINLINE int logn<1>(){return 0;}

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


	MLINLINE float random(float i_min, float i_max)
	{
		return (float)rand()/(float)RAND_MAX*(i_max-i_min)+i_min;
	}

	MLINLINE int  random(int i_min, int i_max)
	{
		return (rand())*(i_max-i_min)/RAND_MAX+i_min;
	}

	MLINLINE float med3(float a1,float a2,float a3)
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
