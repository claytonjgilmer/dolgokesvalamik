#ifndef _vec4_h_
#define _vec4_h_

#include <math.h>

//ezt azert at kellene gondolni

	struct vec4
	{
		float x,y,z,w;
	};//vec4

	MLINLINE void clear(vec4& v)
	{
		v.x=v.y=v.z=0.0f;
		v.w=1.0f; //???
	}

	MLINLINE void set(vec4& v, float i_xcoord, float i_ycoord, float i_zcoord, float i_wcoord=1.0f)
	{
		v.x=i_xcoord;
		v.y=i_ycoord;
		v.z=i_zcoord;
		v.w=i_wcoord;
	}


	MLINLINE void	cross(vec4& dst, const vec4& i_src1, const vec4& i_src2)
	{
		dst.x=i_src1.y*i_src2.z-i_src1.z*i_src2.y;
		dst.y=i_src1.z*i_src2.x-i_src1.x*i_src2.z;
		dst.z=i_src1.x*i_src2.y-i_src1.y*i_src2.x;
		dst.w=1.0f;
	}

	MLINLINE float dot4(const vec4& i_v1, const vec4& i_v2)
	{
		return i_v1.x*i_v2.x+i_v1.y*i_v2.y+i_v1.z*i_v2.z+i_v1.w*i_v2.w;
	}

	MLINLINE float	get_squarelength(const vec4& v)
	{
		return v.x*v.x+v.y*v.y+v.z*v.z;
	}

	MLINLINE float	get_length(const vec4& v)
	{
		return sqrt(get_squarelength(v));
	}

	MLINLINE void	normalize(vec4& v)
	{
		float length=get_length(v);

		v.x/=length;
		v.y/=length;
		v.z/=length;
	}

	MLINLINE void	normalizesafe(vec4 &v)
	{
		float squarelength=get_squarelength(v);

		if (squarelength>0.000001f)
		{
			float length=sqrt(squarelength);

			v.x/=length;
			v.y/=length;
			v.z/=length;
		}
		else
		{
			v.x=v.y=v.z=0;
		}
	}

	MLINLINE vec4 operator+(const vec4& v, const vec4& i_other)
	{
	    vec4 s;
		set(s,v.x+i_other.x,v.y+i_other.y,v.z+i_other.z);
		return s;
	}

	MLINLINE vec4 operator-(const vec4& v, const vec4& i_other)
	{
	    vec4 d;
		set(d,v.x-i_other.x,v.y-i_other.y,v.z-i_other.z);
		return d;
	}

	MLINLINE vec4& operator+=(vec4& v, const vec4& i_other)
	{
		v.x+=i_other.x;
		v.y+=i_other.y;
		v.z+=i_other.z;

		return v;
	}

	MLINLINE vec4& operator-=(vec4& v, const vec4& i_other)
	{
		v.x-=i_other.x;
		v.y-=i_other.y;
		v.z-=i_other.z;
		return v;
	}

	MLINLINE vec4	operator/(const vec4& v, float i_scalar)
	{
	    vec4 d;
		set(d, v.x/i_scalar,v.y/i_scalar,v.z/i_scalar);
		return d;
	}

	MLINLINE vec4 operator*(float s, const vec4& v)
	{
	    vec4 m;
	    set(m, s*v.x, s*v.y, s*v.z);
		return m;
	}

	MLINLINE vec4& operator*=(vec4& v, float i_scalar)
	{
		v.x*=i_scalar;
		v.y*=i_scalar;
		v.z*=i_scalar;
		return v;
	}

	MLINLINE vec4& operator/=(vec4& v, float i_scalar)
	{
		v.x/=i_scalar;
		v.y/=i_scalar;
		v.z/=i_scalar;
		return v;
	}

#endif//_vec4_h_
