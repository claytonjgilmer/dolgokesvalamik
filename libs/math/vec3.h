#ifndef _vec3_h_
#define _vec3_h_

#include "math.h"

	struct vec3
	{
//		vec3(){}
//		vec3(float i_xcoord, float i_ycoord, float i_zcoord){set(i_xcoord,i_ycoord,i_zcoord);}
		float&	operator[](int i_index);
		const float& operator[](int i_index) const;

		void	clear();
		void	set(float i_xcoord, float i_ycoord, float i_zcoord);

		void	cross(const vec3& i_src1, const vec3& i_src2);
		friend
		vec3 cross(const vec3& i_src1, const vec3& i_src2);
		vec3 cross(const vec3& i_other) const;

		float	dot(const vec3& i_other) const;
		friend
		float	dot(const vec3& i_v1, const vec3& i_v2);

		float	squarelength() const;
		float	length() const;
		void	normalize();
		void	normalizesafe();

		void rotate(const vec3& i_src, const vec3& i_axis, float i_angle);

		void	abs(const vec3& i_src);

		vec3	operator+(const vec3& i_other) const;
		vec3	operator-(const vec3& i_other) const;
		void	operator+=(const vec3& i_other);
		void	operator-=(const vec3& i_other);

		vec3	operator*(float i_scalar) const;
		vec3	operator/(float i_scalar) const;

		friend
		vec3 operator*(float i_scalar, const vec3& i_vector);

		void	operator*=(float i_scalar);
		void	operator/=(float i_scalar);

		vec3	operator-() const;

		void set_max(const vec3& i_v1, const vec3& i_v2);
		void set_max(const vec3& i_other);
		void set_min(const vec3& i_v1, const vec3& i_v2);
		void set_min(const vec3& i_other);

		float maxelem() const;

		void interpolate(const vec3& i_src1, const vec3& i_src2, float i_time);


		float x,y,z;
	};//vec3

	MLINLINE float& vec3::operator[](int i_index)
	{
		return *(&x+i_index);
	}

	MLINLINE const float& vec3::operator[](int i_index) const
	{
		return *(&x+i_index);
	}


	MLINLINE void vec3::clear()
	{
		x=y=z=0.0f;
	}

	MLINLINE void vec3::set(float i_xcoord, float i_ycoord, float i_zcoord)
	{
		x=i_xcoord;
		y=i_ycoord;
		z=i_zcoord;
	}


	MLINLINE void	vec3::cross(const vec3& i_src1, const vec3& i_src2)
	{
		x=i_src1.y*i_src2.z-i_src1.z*i_src2.y;
		y=i_src1.z*i_src2.x-i_src1.x*i_src2.z;
		z=i_src1.x*i_src2.y-i_src1.y*i_src2.x;
	}

	MLINLINE vec3 cross(const vec3& i_src1, const vec3& i_src2)
	{
		vec3 c;
		c.set(
			i_src1.y*i_src2.z-i_src1.z*i_src2.y,
			i_src1.z*i_src2.x-i_src1.x*i_src2.z,
			i_src1.x*i_src2.y-i_src1.y*i_src2.x
		);

		return c;
	}

	MLINLINE vec3 vec3::cross(const vec3& i_other) const
	{
		return ::cross(*this,i_other);
	}

	MLINLINE float	vec3::dot(const vec3& i_other) const
	{
		return x*i_other.x+y*i_other.y+z*i_other.z;
	}

	MLINLINE float dot(const vec3& i_v1, const vec3& i_v2)
	{
		return i_v1.x*i_v2.x+i_v1.y*i_v2.y+i_v1.z*i_v2.z;
	}

	MLINLINE float	vec3::squarelength() const
	{
		return x*x+y*y+z*z;
	}

	MLINLINE float	vec3::length() const
	{
		return sqrt(squarelength());
	}

	MLINLINE void	vec3::normalize()
	{
		float l=length();

		x/=l;
		y/=l;
		z/=l;
	}

	MLINLINE void	vec3::normalizesafe()
	{
		float sqlength=squarelength();

		if (sqlength>0.000001f)
		{
			float l=sqrt(sqlength);

			x/=l;
			y/=l;
			z/=l;
		}
		else
		{
			x=y=z=0;
		}
	}

	MLINLINE vec3	vec3::operator+(const vec3& i_other) const
	{
		vec3 a;
		a.set(x+i_other.x,y+i_other.y,z+i_other.z);
		return a;
	}

	MLINLINE vec3	vec3::operator-(const vec3& i_other) const
	{
		vec3 d;
		d.set(x-i_other.x,y-i_other.y,z-i_other.z);
		return d;
	}

	MLINLINE void	vec3::operator+=(const vec3& i_other)
	{
		x+=i_other.x;
		y+=i_other.y;
		z+=i_other.z;
	}

	MLINLINE void	vec3::operator-=(const vec3& i_other)
	{
		x-=i_other.x;
		y-=i_other.y;
		z-=i_other.z;
	}

	MLINLINE vec3	vec3::operator*(float i_scalar) const
	{
		vec3 m; m.set(x*i_scalar,y*i_scalar,z*i_scalar);
		return m;
	}

	MLINLINE vec3	vec3::operator/(float i_scalar) const
	{
		vec3 d;
		d.set(x/i_scalar,y/i_scalar,z/i_scalar);
		return d;
	}

	MLINLINE vec3 operator*(float i_scalar, const vec3& i_vector)
	{
		return i_vector*i_scalar;
	}

	MLINLINE void	vec3::operator*=(float i_scalar)
	{
		x*=i_scalar;
		y*=i_scalar;
		z*=i_scalar;
	}

	MLINLINE void	vec3::operator/=(float i_scalar)
	{
		x/=i_scalar;
		y/=i_scalar;
		z/=i_scalar;
	}

	MLINLINE vec3	vec3::operator-() const
	{
		vec3 n; n.set(-x,-y,-z); return n;
	}

	MLINLINE void vec3::abs(const vec3& i_src)
	{
		x=fabsf(i_src.x);
		y=fabsf(i_src.y);
		z=fabsf(i_src.z);
	}

	MLINLINE void vec3::set_max(const vec3& i_v1, const vec3& i_v2)
	{
		x=i_v1.x > i_v2.x ? i_v1.x : i_v2.x;
		y=i_v1.y > i_v2.y ? i_v1.y : i_v2.y;
		z=i_v1.z > i_v2.z ? i_v1.z : i_v2.z;
	}

	MLINLINE void vec3::set_max(const vec3& i_other)
	{
		if (i_other.x>x)
			x=i_other.x;
		if (i_other.y>y)
			y=i_other.y;
		if (i_other.z>z)
			z=i_other.z;
	}

	MLINLINE float vec3::maxelem() const
	{
		float r=x;
		if (y>r) r=y;
		if (z>r) r=z;

		return r;
	}

	MLINLINE void vec3::set_min(const vec3& i_v1, const vec3& i_v2)
	{
		x=i_v1.x < i_v2.x ? i_v1.x : i_v2.x;
		y=i_v1.y < i_v2.y ? i_v1.y : i_v2.y;
		z=i_v1.z < i_v2.z ? i_v1.z : i_v2.z;
	}

	MLINLINE void vec3::set_min(const vec3& i_other)
	{
		if (i_other.x<x)
			x=i_other.x;
		if (i_other.y<y)
			y=i_other.y;
		if (i_other.z<z)
			z=i_other.z;
	}

	MLINLINE void vec3::rotate(const vec3& i_src, const vec3& i_axis, float i_angle)
	{
		vec3 a=i_src.dot(i_axis)*i_axis;
		vec3 b=i_src-a;
		vec3 c; c.cross(i_axis,b);
		*this=b*cosf(i_angle)+c*sinf(i_angle)+a;
	}

	MLINLINE void getorthogonalaxes(vec3& o_axis1, vec3& o_axis2, const vec3& i_axissrc)
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

	MLINLINE void vec3::interpolate(const vec3& i_src1, const vec3& i_src2, float i_time)
	{
		x=i_src1.x+i_time*(i_src2.x-i_src1.x);
		y=i_src1.y+i_time*(i_src2.y-i_src1.y);
		z=i_src1.z+i_time*(i_src2.z-i_src1.z);
	}

	MLINLINE float scalartriple(vec3 i_v1, vec3 i_v2, vec3 i_v3)
	{
		return dot(i_v1,cross(i_v2,i_v3));
	}

	MLINLINE vec3 add(const vec3& v1, const vec3& v2)
	{
		vec3 sum;
		sum.x=v1.x+v2.x;
		sum.y=v1.y+v2.y;
		sum.z=v1.z+v2.z;

		return sum;
	}
#endif//_vec3_h_
