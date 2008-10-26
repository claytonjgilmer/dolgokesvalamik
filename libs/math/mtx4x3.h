#ifndef _mtx4x3_h_
#define _mtx4x3_h_

#include "mtx3x3.h"


	//////////////////////////////////////////////////////////////////////////
	//			mtx4x3
	//////////////////////////////////////////////////////////////////////////

	class mtx4x3:public mtx3x3
	{
	public:
		void identity();
		static mtx4x3 identitymtx();

		void transform(vec3& o_dst, const vec3& i_src) const;
		void transform2(vec3& o_dst, const vec3& i_src) const;
		vec3 transform(const vec3& i_src) const;
		void transformtransposed(vec3& o_dst, const vec3& i_src) const;
		vec3 transformtransposed(const vec3& i_src) const;

		void multiply(const mtx4x3& i_mtx1, const mtx4x3& i_mtx2);
		void multiplytransposed(const mtx4x3& i_src1, const mtx4x3& i_src2transposed);

		void invert(const mtx4x3& i_src);
		void linearinvert(const mtx4x3& i_src);

		void interpolate(const mtx4x3& i_src1, const mtx4x3& i_src2, float i_time);

		void normalize();
		void rotate(const mtx4x3& i_src, const vec3& i_axis, float i_angle);

		void set_euler(float i_x,float i_y, float i_z);
		
		union
		{
			struct  
			{
				float	_41,_42,_43;
			};

			struct  
			{
				vec3 t;
			};
		};
	}; //class mtx4x3

	MLINLINE void mtx4x3::identity()
	{
		mtx3x3::identity();
		_41=_42=_43=0.0f;
	}

	MLINLINE mtx4x3 mtx4x3::identitymtx()
	{
		mtx4x3 ret; ret.identity(); return ret;
	}

	MLINLINE void mtx4x3::transform(vec3& o_dst, const vec3& i_src) const
	{
		o_dst.x=i_src.x*_11+i_src.y*_21+i_src.z*_31+_41;
		o_dst.y=i_src.x*_12+i_src.y*_22+i_src.z*_32+_42;
		o_dst.z=i_src.x*_13+i_src.y*_23+i_src.z*_33+_43;
	}

	MLINLINE void mtx4x3::transform2(vec3& o_dst, const vec3& i_src) const
	{
//		o_dst[0]=i_src[0]*_11+i_src[1]*_21+i_src[2]*_31+_41;
//		o_dst[1]=i_src[0]*_12+i_src[1]*_22+i_src[2]*_32+_42;
//		o_dst[2]=i_src[0]*_13+i_src[1]*_23+i_src[2]*_33+_43;
		o_dst=t+i_src.x*x+i_src.y*y+i_src.z*z;

//		o_dst.x=i_src.x*_11;
//		o_dst.y=i_src.x*_12;
//		o_dst.z=i_src.x*_13;
//		o_dst.x+=i_src.y*_21;
//		o_dst.y+=i_src.y*_22;
//		o_dst.z+=i_src.y*_23;
//		o_dst.x+=i_src.z*_31;
//		o_dst.y+=i_src.z*_32;
//		o_dst.z+=i_src.z*_33;
//		o_dst.x+=_41;
//		o_dst.y+=_42;
//		o_dst.z+=_43;
	}

	MLINLINE vec3 mtx4x3::transform(const vec3& i_src) const
	{
		return vec3
		(
		i_src.x*_11+i_src.y*_21+i_src.z*_31+_41,
		i_src.x*_12+i_src.y*_22+i_src.z*_32+_42,
		i_src.x*_13+i_src.y*_23+i_src.z*_33+_43
		);

	}

	MLINLINE void mtx4x3::transformtransposed(vec3& o_dst, const vec3& i_src) const
	{
		transformtransposed3x3(o_dst,i_src-t);
	}

	MLINLINE vec3 mtx4x3::transformtransposed(const vec3& i_src) const
	{
		return transformtransposed3x3(i_src-t);
	}

	MLINLINE void mtx4x3::multiply(const mtx4x3& i_src1, const mtx4x3& i_src2)
	{
		_11=i_src1._11*i_src2._11+i_src1._12*i_src2._21+i_src1._13*i_src2._31;
		_12=i_src1._11*i_src2._12+i_src1._12*i_src2._22+i_src1._13*i_src2._32;
		_13=i_src1._11*i_src2._13+i_src1._12*i_src2._23+i_src1._13*i_src2._33;

		_21=i_src1._21*i_src2._11+i_src1._22*i_src2._21+i_src1._23*i_src2._31;
		_22=i_src1._21*i_src2._12+i_src1._22*i_src2._22+i_src1._23*i_src2._32;
		_23=i_src1._21*i_src2._13+i_src1._22*i_src2._23+i_src1._23*i_src2._33;

		_31=i_src1._31*i_src2._11+i_src1._32*i_src2._21+i_src1._33*i_src2._31;
		_32=i_src1._31*i_src2._12+i_src1._32*i_src2._22+i_src1._33*i_src2._32;
		_33=i_src1._31*i_src2._13+i_src1._32*i_src2._23+i_src1._33*i_src2._33;

		_41=i_src1._41*i_src2._11+i_src1._42*i_src2._21+i_src1._43*i_src2._31+i_src2._41;
		_42=i_src1._41*i_src2._12+i_src1._42*i_src2._22+i_src1._43*i_src2._32+i_src2._42;
		_43=i_src1._41*i_src2._13+i_src1._42*i_src2._23+i_src1._43*i_src2._33+i_src2._43;
	}

	MLINLINE void mtx4x3::multiplytransposed(const mtx4x3& i_src1, const mtx4x3& i_srctransposed)
	{
		this->x=i_srctransposed.transformtransposed3x3(i_src1.x);
		this->y=i_srctransposed.transformtransposed3x3(i_src1.y);
		this->z=i_srctransposed.transformtransposed3x3(i_src1.z);
		this->t=i_srctransposed.transformtransposed(i_src1.t);
	}


	MLINLINE void mtx4x3::invert(const mtx4x3& i_src)
	{
		float d11 = i_src._22*i_src._33 - i_src._23*i_src._32;
		float d12 = i_src._23*i_src._31 - i_src._21*i_src._33;
		float d13 = i_src._21*i_src._32 - i_src._22*i_src._31;
		float det = i_src._11*d11 + i_src._12*d12 + i_src._13*d13;
		float id = 1.0f / det;
		float id_11 = id * i_src._11;
		float id_12 = id * i_src._12;
		float id_13 = id * i_src._13;
		_11 = id * d11;
		_12 = id_13*i_src._32 - id_12*i_src._33;
		_13 = id_12*i_src._23 - id_13*i_src._22;
		_21 = id * d12;
		_22 = id_11*i_src._33 - id_13*i_src._31;
		_23 = id_13*i_src._21 - id_11*i_src._23;
		_31 = id * d13;
		_32 = id_12*i_src._31 - id_11*i_src._32;
		_33 = id_11*i_src._22 - id_12*i_src._21;
		_41 = -(i_src._41*_11 + i_src._42*_21 + i_src._43*_31);
		_42 = -(i_src._41*_12 + i_src._42*_22 + i_src._43*_32);
		_43 = -(i_src._41*_13 + i_src._42*_23 + i_src._43*_33);
	}

	MLINLINE void mtx4x3::linearinvert(const mtx4x3& i_src)
	{
		x=i_src.x/i_src.x.squarelength();
		y=i_src.y/i_src.y.squarelength();
		z=i_src.z/i_src.z.squarelength();

		transpose3x3();

		transform3x3(t,-i_src.t);
	}

	MLINLINE void mtx4x3::normalize()
	{
		this->z.normalize();
		this->x.cross(this->y,this->z);
		this->x.normalize();
		this->y.cross(this->z,this->x);

	}

	MLINLINE void mtx4x3::rotate(const mtx4x3& i_src, const vec3& i_axis, float i_angle)
	{
		this->z.rotate(i_src.z,i_axis,i_angle);
		this->y.rotate(i_src.y,i_axis,i_angle);
		normalize();
	}

	MLINLINE void mtx4x3::interpolate(const mtx4x3& i_src1, const mtx4x3& i_src2, float i_time)
	{
		this->x.interpolate(i_src1.x, i_src2.x, i_time);
		this->y.interpolate(i_src1.y, i_src2.y, i_time);
		this->z.interpolate(i_src1.z, i_src2.z, i_time);
		this->t.interpolate(i_src1.t, i_src2.t, i_time);
	}

	MLINLINE void mtx4x3::set_euler(float i_x,float i_y, float i_z)
	{
		float sx=(sin(i_x)), cx=(cos(i_x));
		float sy=(sin(i_y)), cy=(cos(i_y));
		float sz=(sin(i_z)), cz=(cos(i_z));

		_11 = cz*cy-sz*sx*sy;	_12 = sz*cx;	_13 = -sz*sx*cy-cz*sy;
		_21 = -cz*sx*sy-sz*cy;	_22 = cz*cx;	_23 = sz*sy-cz*sx*cy;
		_31 = cx*sy;			_32 = sx;		_33 = cx*cy;
	}
#endif//_mtx4x3_h_