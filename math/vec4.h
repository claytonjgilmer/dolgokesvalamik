#ifndef _vec4_h_
#define _vec4_h_

#include <math.h>

//ezt azert at kellene gondolni

namespace math
{
	class vec4
	{
	public:
		vec4();
		vec4(float i_xcoord, float i_ycoord, float i_zcoord, float i_wcoord=1.0f);

		float&	operator[](int i_index);
		const float& operator[](int i_index) const;

		void	clear();
		void	set(float i_xcoord, float i_ycoord, float i_zcoord, float i_wcoord=1.0f);

		void	cross(const vec4& i_src1, const vec4& i_src2);
		friend 
		vec4 cross(const vec4& i_src1, const vec4& i_src2);
		vec4 cross(const vec4& i_other) const;
		
		float	dot(const vec4& i_other) const;
		friend
		float	dot(const vec4& i_v1, const vec4& i_v2);

		float	squarelength() const;
		float	length() const;
		void	normalize();
		void	normalizesafe();

		void rotate(const vec4& i_src, const vec4& i_axis, float i_angle);

		void	abs(const vec4& i_src);

		vec4	operator+(const vec4& i_other) const;
		vec4	operator-(const vec4& i_other) const;
		void	operator+=(const vec4& i_other);
		void	operator-=(const vec4& i_other);

		vec4	operator*(float i_scalar) const;
		vec4	operator/(float i_scalar) const;

		friend
		vec4 operator*(float i_scalar, const vec4& i_vector);

		void	operator*=(float i_scalar);
		void	operator/=(float i_scalar);

		vec4	operator-() const;

		void max(const vec4& i_v1, const vec4& i_v2);
		void max(const vec4& i_other);
		void min(const vec4& i_v1, const vec4& i_v2);
		void min(const vec4& i_other);

		float maxelem() const;

		void interpolate(const vec4& i_src1, const vec4& i_src2, float i_time);


		float x,y,z;
	};//vec4

	MLINLINE vec4::vec4()
	{
	}

	MLINLINE vec4::vec4(float i_xcoord, float i_ycoord, float i_zcoord, float i_wcoord)
	{
		x=i_xcoord;
		y=i_ycoord;
		z=i_zcoord;
		w=i_wcoord
	}

	MLINLINE float& vec4::operator[](int i_index)
	{
		return *(((float*)this)+i_index);
	}

	MLINLINE const float& vec4::operator[](int i_index) const
	{
		return *(((float*)this)+i_index);
	}


	MLINLINE void vec4::clear()
	{
		x=y=z=0.0f;
		w=1.0f; //???
	}

	MLINLINE void vec4::set(float i_xcoord, float i_ycoord, float i_zcoord, float i_wcoord)
	{
		x=i_xcoord;
		y=i_ycoord;
		z=i_zcoord;
		w=i_wcoord;
	}


	MLINLINE void	vec4::cross(const vec4& i_src1, const vec4& i_src2)
	{
		x=i_src1.y*i_src2.z-i_src1.z*i_src2.y;
		y=i_src1.z*i_src2.x-i_src1.x*i_src2.z;
		z=i_src1.x*i_src2.y-i_src1.y*i_src2.x;
		w=1.0f;
	}

	MLINLINE vec4 cross(const vec4& i_src1, const vec4& i_src2)
	{
		return vec4(
			i_src1.y*i_src2.z-i_src1.z*i_src2.y,
			i_src1.z*i_src2.x-i_src1.x*i_src2.z,
			i_src1.x*i_src2.y-i_src1.y*i_src2.x
			);
	}

	MLINLINE vec4 vec4::cross(const vec4& i_other) const
	{
		return math::cross(*this,i_other);
	}

	MLINLINE float	vec4::dot(const vec4& i_other) const
	{
		return x*i_other.x+y*i_other.y+z*i_other.z;
	}

	MLINLINE float dot(const vec4& i_v1, const vec4& i_v2)
	{
		return i_v1.x*i_v2.x+i_v1.y*i_v2.y+i_v1.z*i_v2.z;
	}

	MLINLINE float	vec4::squarelength() const
	{
		return x*x+y*y+z*z;
	}

	MLINLINE float	vec4::length() const
	{
		return sqrt(squarelength());
	}

	MLINLINE void	vec4::normalize()
	{
		float length=length();

		x/=length;
		y/=length;
		z/=length;
	}

	MLINLINE void	vec4::normalizesafe()
	{
		float squarelength=squarelength();

		if (squarelength>0.000001f)
		{
			float length=sqrt(squarelength);

			x/=length;
			y/=length;
			z/=length;
		}
		else
		{
			x=y=z=0;
		}
	}

	MLINLINE vec4	vec4::operator+(const vec4& i_other) const
	{
		return vec4(x+i_other.x,y+i_other.y,z+i_other.z);
	}

	MLINLINE vec4	vec4::operator-(const vec4& i_other) const
	{
		return vec4(x-i_other.x,y-i_other.y,z-i_other.z);
	}

	MLINLINE void	vec4::operator+=(const vec4& i_other)
	{
		x+=i_other.x;
		y+=i_other.y;
		z+=i_other.z;
	}

	MLINLINE void	vec4::operator-=(const vec4& i_other)
	{
		x-=i_other.x;
		y-=i_other.y;
		z-=i_other.z;
	}

	MLINLINE vec4	vec4::operator*(float i_scalar) const
	{
		return vec4(x*i_scalar,y*i_scalar,z*i_scalar);
	}

	MLINLINE vec4	vec4::operator/(float i_scalar) const
	{
		return vec4(x/i_scalar,y/i_scalar,z/i_scalar);
	}

	MLINLINE vec4 operator*(float i_scalar, const vec4& i_vector)
	{
		return i_vector*i_scalar;
	}

	MLINLINE void	vec4::operator*=(float i_scalar)
	{
		x*=i_scalar;
		y*=i_scalar;
		z*=i_scalar;
	}

	MLINLINE void	vec4::operator/=(float i_scalar)
	{
		x/=i_scalar;
		y/=i_scalar;
		z/=i_scalar;
	}

	MLINLINE vec4	vec4::operator-() const
	{
		return vec4(-x,-y,-z);
	}

	MLINLINE void vec4::abs(const vec4& i_src)
	{
		x=fabsf(i_src.x);
		y=fabsf(i_src.y);
		z=fabsf(i_src.z);
	}

	MLINLINE void vec4::max(const vec4& i_v1, const vec4& i_v2)
	{
		x=i_v1.x > i_v2.x ? i_v1.x : i_v2.x;
		y=i_v1.y > i_v2.y ? i_v1.y : i_v2.y;
		z=i_v1.z > i_v2.z ? i_v1.z : i_v2.z;
	}

	MLINLINE void vec4::max(const vec4& i_other)
	{
		if (i_other.x>x)
			x=i_other.x;
		if (i_other.y>y)
			y=i_other.y;
		if (i_other.z>z)
			z=i_other.z;
	}

	MLINLINE float vec4::maxelem() const
	{
		float r=x;
		if (y>r) r=y;
		if (z>r) r=z;

		return r;
	}

	MLINLINE void vec4::min(const vec4& i_v1, const vec4& i_v2)
	{
		x=i_v1.x < i_v2.x ? i_v1.x : i_v2.x;
		y=i_v1.y < i_v2.y ? i_v1.y : i_v2.y;
		z=i_v1.z < i_v2.z ? i_v1.z : i_v2.z;
	}

	MLINLINE void vec4::min(const vec4& i_other)
	{
		if (i_other.x<x)
			x=i_other.x;
		if (i_other.y<y)
			y=i_other.y;
		if (i_other.z<z)
			z=i_other.z;
	}

	MLINLINE void vec4::rotate(const vec4& i_src, const vec4& i_axis, float i_angle)
	{
		vec4 a=i_src.dot(i_axis)*i_axis;
		vec4 b=i_src-a;
		vec4 c; c.cross(i_axis,b);
		*this=b*cosf(i_angle)+c*sinf(i_angle)+a;
	}

	MLINLINE void getorthogonalaxes(vec4& o_axis1, vec4& o_axis2, const vec4& i_axissrc)
	{
		if ((i_axissrc.z) > 0.707f || i_axissrc.z<-0.707f)
		{
			// y-z plane
			float  a = i_axissrc.y*i_axissrc.y + i_axissrc.z*i_axissrc.z;
			float k = 1/sqrt(a);
			o_axis1.x = 0;
			o_axis1.y = -i_axissrc.z*k;
			o_axis1.z = i_axissrc.y*k;

			o_axis2.x = a*k;
			o_axis2.y = -i_axissrc.x*o_axis1.z;
			o_axis2.z = i_axissrc.x*o_axis1.y;
		}
		else
		{
			// x-y plane
			float a = i_axissrc.x*i_axissrc.x + i_axissrc.y*i_axissrc.y;
			float k = 1/sqrt(a);
			o_axis1.x = -i_axissrc.y*k;
			o_axis1.y = i_axissrc.x*k;
			o_axis1.z = 0;

			o_axis2.x = -i_axissrc.z*o_axis1.y;
			o_axis2.y = i_axissrc.z*o_axis1.x;
			o_axis2.z = a*k;
		}
	}

	MLINLINE void vec4::interpolate(const vec4& i_src1, const vec4& i_src2, float i_time)
	{
		x=i_src1.x+i_time*(i_src2.x-i_src1.x);
		y=i_src1.y+i_time*(i_src2.y-i_src1.y);
		z=i_src1.z+i_time*(i_src2.z-i_src1.z);
	}

}//namespace math
#endif//_vec4_h_