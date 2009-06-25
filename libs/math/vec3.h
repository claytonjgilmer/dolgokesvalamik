#ifndef _vec3_h_
#define _vec3_h_

#include "math.h"

	struct vec3;
#ifdef _DEBUG
	void check_vec3(const vec3& v);
#else
#define check_vec3(_V_)
#endif
	struct vec3
	{
		vec3(){}
		vec3(f32 i_xcoord, f32 i_ycoord, f32 i_zcoord): x(i_xcoord),y(i_ycoord),z(i_zcoord){}
		f32&	operator[](int i_index);
		const f32& operator[](int i_index) const;

		void	clear();
		void	set(f32 i_xcoord, f32 i_ycoord, f32 i_zcoord);

		void	cross(const vec3& i_src1, const vec3& i_src2);
		friend
		vec3 cross(const vec3& i_src1, const vec3& i_src2);
		vec3 cross(const vec3& i_other) const;

		f32	dot(const vec3& i_other) const;
		friend
		f32	dot(const vec3& i_v1, const vec3& i_v2);

		f32	squarelength() const;
		f32	length() const;
		void	normalize();
		void	normalizesafe();

		void rotate(const vec3& i_src, const vec3& i_axis, f32 i_angle);
		void rotate(const vec3& i_axis, f32 sinangle, f32 cosangle);

		void	abs(const vec3& i_src);

		vec3	operator+(const vec3& i_other) const;
		vec3	operator-(const vec3& i_other) const;
		void	operator+=(const vec3& i_other);
		void	operator-=(const vec3& i_other);
/*
		void operator=(const vec3& v)
		{
			check_vec3(v);
			x=v.x;
			y=v.y;
			z=v.z;
		}
*/
		vec3	operator*(f32 i_scalar) const;
		vec3	operator/(f32 i_scalar) const;

		friend
		vec3 operator*(f32 i_scalar, const vec3& i_vector);

		void	operator*=(f32 i_scalar);
		void	operator/=(f32 i_scalar);

		vec3	operator-() const;

		void set_max(const vec3& i_v1, const vec3& i_v2);
		void set_max(const vec3& i_other);
		void set_min(const vec3& i_v1, const vec3& i_v2);
		void set_min(const vec3& i_other);

		f32 maxelem() const;

		void interpolate(const vec3& i_src1, const vec3& i_src2, f32 i_time);


		f32 x,y,z;
	};//vec3

	MLINLINE f32& vec3::operator[](int i_index)
	{
		return *(&x+i_index);
	}

	MLINLINE const f32& vec3::operator[](int i_index) const
	{
		return *(&x+i_index);
	}


	MLINLINE void vec3::clear()
	{
		x=y=z=0.0f;
	}

	MLINLINE void vec3::set(f32 i_xcoord, f32 i_ycoord, f32 i_zcoord)
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
		return vec3(
			i_src1.y*i_src2.z-i_src1.z*i_src2.y,
			i_src1.z*i_src2.x-i_src1.x*i_src2.z,
			i_src1.x*i_src2.y-i_src1.y*i_src2.x
		);
	}

	MLINLINE vec3 vec3::cross(const vec3& i_other) const
	{
		return ::cross(*this,i_other);
	}

	MLINLINE f32	vec3::dot(const vec3& i_other) const
	{
		return x*i_other.x+y*i_other.y+z*i_other.z;
	}

	MLINLINE f32 dot(const vec3& i_v1, const vec3& i_v2)
	{
		return i_v1.x*i_v2.x+i_v1.y*i_v2.y+i_v1.z*i_v2.z;
	}

	MLINLINE f32	vec3::squarelength() const
	{
		return x*x+y*y+z*z;
	}

	MLINLINE f32	vec3::length() const
	{
		return sqrt(squarelength());
	}

	MLINLINE void	vec3::normalize()
	{
		f32 l=length();

		x/=l;
		y/=l;
		z/=l;
	}

	MLINLINE void	vec3::normalizesafe()
	{
		f32 sqlength=squarelength();

		if (sqlength>0.000001f)
		{
			f32 l=sqrt(sqlength);

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
		return vec3(x+i_other.x,y+i_other.y,z+i_other.z);
	}

	MLINLINE vec3	vec3::operator-(const vec3& i_other) const
	{
		return vec3(x-i_other.x,y-i_other.y,z-i_other.z);
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

	MLINLINE vec3	vec3::operator*(f32 i_scalar) const
	{
		return vec3(x*i_scalar,y*i_scalar,z*i_scalar);
	}

	MLINLINE vec3	vec3::operator/(f32 i_scalar) const
	{
		return vec3(x/i_scalar,y/i_scalar,z/i_scalar);
	}

	MLINLINE vec3 operator*(f32 i_scalar, const vec3& i_vector)
	{
		return i_vector*i_scalar;
	}

	MLINLINE void	vec3::operator*=(f32 i_scalar)
	{
		x*=i_scalar;
		y*=i_scalar;
		z*=i_scalar;
	}

	MLINLINE void	vec3::operator/=(f32 i_scalar)
	{
		x/=i_scalar;
		y/=i_scalar;
		z/=i_scalar;
	}

	MLINLINE vec3	vec3::operator-() const
	{
		return vec3(-x,-y,-z);
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

	MLINLINE f32 vec3::maxelem() const
	{
		f32 r=x;
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

	MLINLINE void vec3::rotate(const vec3& i_src, const vec3& i_axis, f32 i_angle)
	{
		vec3 a=i_src.dot(i_axis)*i_axis;
		vec3 b=i_src-a;
		vec3 c; c.cross(i_axis,b);
		*this=b*cosf(i_angle)+c*sinf(i_angle)+a;
	}

	MLINLINE void vec3::rotate(const vec3& i_axis, f32 sinangle, f32 cosangle)
	{
		vec3 a=this->dot(i_axis)*i_axis;
		vec3 b=*this-a;
		vec3 c; c.cross(i_axis,b);
		*this=b*cosangle+c*sinangle+a;
	}

	MLINLINE void getorthogonalaxes(vec3& o_axis1, vec3& o_axis2, const vec3& i_axissrc)
	{
		if ((i_axissrc.z) > 0.707f || i_axissrc.z<-0.707f)
		{
			// y-z plane
			f32  a = i_axissrc.y*i_axissrc.y + i_axissrc.z*i_axissrc.z;
			f32 k = 1/sqrt(a);
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
			f32 a = i_axissrc.x*i_axissrc.x + i_axissrc.y*i_axissrc.y;
			f32 k = 1/sqrt(a);
			o_axis1.x = -i_axissrc.y*k;
			o_axis1.y = i_axissrc.x*k;
			o_axis1.z = 0;

			o_axis2.x = -i_axissrc.z*o_axis1.y;
			o_axis2.y = i_axissrc.z*o_axis1.x;
			o_axis2.z = a*k;
		}
	}

	MLINLINE void vec3::interpolate(const vec3& i_src1, const vec3& i_src2, f32 i_time)
	{
		x=i_src1.x+i_time*(i_src2.x-i_src1.x);
		y=i_src1.y+i_time*(i_src2.y-i_src1.y);
		z=i_src1.z+i_time*(i_src2.z-i_src1.z);
	}

	MLINLINE f32 scalartriple(vec3 i_v1, vec3 i_v2, vec3 i_v3)
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

	MLINLINE f32 get_tetrahedron_volume(vec3 v1, vec3 v2, vec3 v3, vec3 v4)
	{
		vec3 e1,e2,e3;
		e1=v2-v1;
		e2=v3-v1;
		e3=v4-v1;

		return fabsf(dot(e1,cross(e2,e3)))/6;
	}

	MLINLINE vec3 to_vec3(f32 x,f32 y, f32 z)
	{
		vec3 v; v.set(x,y,z);
		return v;
	}

#ifdef _DEBUG
	MLINLINE void check_vec3(const vec3& v)
	{
		check_float(v.x),check_float(v.y),check_float(v.z);
	}
#endif


#endif//_vec3_h_
