#ifndef _math_h_
#define _math_h_

#include <math.h>

#define myinline __forceinline

namespace math
{
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



	myinline float pi()
	{
		return 3.1415926536f;
	}

	myinline float twopi()
	{
		return 2.0f*pi();
	}

	myinline float pihalf()
	{
		return 0.5f*pi();
	}

	myinline float sin(float i_value)
	{
		return sinf(i_value);
	}

	myinline float cos(float i_value)
	{
		return cosf(i_value);
	}

	myinline float abs(float i_value)
	{
		return fabsf(i_value);
	}

	myinline float asin(float i_value)
	{
		return asinf(i_value);
	}

	myinline float acos(float i_value)
	{
		return acosf(i_value);
	}

	myinline float atan2(float i_value1,float i_value2)
	{
		return atan2f(i_value1,i_value2);
	}

	myinline float sqrt(float i_value)
	{
		return sqrtf(i_value);
	}

	myinline float pow(float i_value, float i_power)
	{
		return powf(i_value,i_power);
	}

	template<class t> t sqr(t i_value)
	{
		return i_value*i_value;
	}

	myinline float radtodegree(float i_degree)
	{
		return i_degree*(180.0f/pi());
	}

	myinline float degreetorad(float i_radian)
	{
		return i_radian*(pi()/180.0f);
	}

	template<class t> myinline t min(t i_value1, t i_value2)
	{
		return i_value1<i_value2 ? i_value1 : i_value2;
	}

	template<class t> myinline t max(t i_value1, t i_value2)
	{
		return i_value1>i_value2 ? i_value1 : i_value2;
	}

	template<class t> myinline t clamp(t i_src, t i_minvalue, t i_maxvalue)
	{
		if (i_src<i_minvalue)
			return i_minvalue;

		if (i_src>i_maxvalue)
			return i_maxvalue;

		return i_src;

	}

	myinline float sign(float i_value)
	{
		return i_value>0.0f ? 1.0f : -1.0f;
	}

	myinline double sign(double i_value)
	{
		return i_value>0.0 ? 1.0 : -1.0;
	}

	myinline void swap(int& io_value1, int& io_value2)
	{
		io_value1^=io_value2;
		io_value2^=io_value1;
		io_value1^=io_value2;
	}

	myinline void swap(float& io_value1, float& io_value2)
	{
		swap((int&)io_value1,(int&)io_value2);
	}

	template<class t>
	myinline void swap(t& io_value1, t& io_value2)
	{
		t tmp=io_value1;
		io_value1=io_value2;
		io_value2=tmp;
	}

	template<class t>
	myinline void sort(t& id0, t& id1)
	{
		if(id0>id1)	std::swap(id0, id1);
	}



	myinline unsigned nextpoweroftwo(unsigned i_value)
	{
		i_value|= (i_value >> 1);
		i_value|= (i_value >> 2);
		i_value|= (i_value >> 4);
		i_value|= (i_value >> 8);
		i_value|= (i_value >> 16);
		return i_value+1;
	}

	myinline int ispoweroftwo(unsigned i_value)
	{
		return !(i_value & (i_value-1));
	}

	myinline int nextnumbermodulo3(int i_actnumber)
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

	template <int n> myinline int logn(){return 1+logn<(n >> 1)>();}
	template<> myinline int logn<1>(){return 0;}



}
#endif// _math_h_