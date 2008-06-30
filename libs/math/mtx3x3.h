#ifndef _mtx3x3_h_
#define _mtx3x3_h_

#include "vec3.h"

namespace math
{
	//////////////////////////////////////////////////////////////////////////
	//			mtx3x3
	//////////////////////////////////////////////////////////////////////////

	class mtx3x3
	{
	public:
		mtx3x3();
		mtx3x3(const vec3& i_xaxis, const vec3& i_yaxis, const vec3& i_zaxis);

		void transpose3x3();
		void transpose3x3(const mtx3x3& i_src);

		float* operator()();
		const float* operator()() const;

		vec3& axisx();
		const vec3& axisx() const;
		vec3& axisy();
		const vec3& axisy() const;
		vec3& axisz();
		const vec3& axisz() const;
		vec3& axis(int i_index);
		const vec3& axis(int i_index) const;

		void identity();

		void fromeuler(float i_xangle, float i_yangle, float i_zangle);
		void geteuler(float& o_xangle, float& o_yangle, float& o_zangle) const;

		void transform3x3(vec3& o_dst, const vec3& i_src) const;
		vec3 transform3x3(const vec3& i_src) const;

		void transformtransposed3x3(vec3& o_dst, const vec3& i_src) const;
		vec3 transformtransposed3x3(const vec3& i_src) const;

		void invert3x3(const mtx3x3& i_src);

		void abs(const mtx3x3& i_src);

		void multiply3x3(const mtx3x3& i_src1, const mtx3x3& i_src2);
		void multiplytransposed3x3(const mtx3x3& i_src1, const mtx3x3& i_src2transposed);


		union
		{
			struct
			{
				float	_11,_12,_13;
				float	_21,_22,_23;
				float	_31,_32,_33;
			};

			struct  
			{
				vec3 xaxis,yaxis,zaxis;
			};
		};
	}; //class mtx3x3

	inline mtx3x3::mtx3x3()
	{
	}

	inline mtx3x3::mtx3x3(const vec3& i_xaxis, const vec3& i_yaxis, const vec3& i_zaxis)
	{
		xaxis=i_xaxis;
		yaxis=i_yaxis;
		zaxis=i_zaxis;
	}
	
	inline vec3& mtx3x3::axisx()
	{
		return xaxis;
	}

	inline const vec3& mtx3x3::axisx() const
	{
		return xaxis;
	}

	inline vec3& mtx3x3::axisy()
	{
		return yaxis;
	}

	inline const vec3& mtx3x3::axisy() const
	{
		return yaxis;
	}

	inline vec3& mtx3x3::axisz()
	{
		return zaxis;
	}

	inline const vec3& mtx3x3::axisz() const
	{
		return zaxis;
	}

	inline vec3& mtx3x3::axis(int i_index)
	{
		return *(((vec3*)&_11)+i_index);
	}

	inline const vec3& mtx3x3::axis(int i_index) const
	{
		return *(((vec3*)&_11)+i_index);
	}


	inline void mtx3x3::identity()
	{
		_11=_22=_33=1.0f;
		_12=_13=_21=_23=_31=_32=0.0f;
	}

	inline void mtx3x3::transform3x3(vec3& o_dst, const vec3& i_src) const
	{
		o_dst.x=i_src.x*_11+i_src.y*_21+i_src.z*_31;
		o_dst.y=i_src.x*_12+i_src.y*_22+i_src.z*_32;
		o_dst.z=i_src.x*_13+i_src.y*_23+i_src.z*_33;
	}

	inline vec3 mtx3x3::transform3x3(const vec3& i_src) const
	{
		return vec3
			(
			i_src.x*_11+i_src.y*_21+i_src.z*_31,
			i_src.x*_12+i_src.y*_22+i_src.z*_32,
			i_src.x*_13+i_src.y*_23+i_src.z*_33
			);
	}

	inline void mtx3x3::transformtransposed3x3(vec3& o_dst, const vec3& i_src) const
	{
		o_dst.x=dot(i_src,xaxis);
		o_dst.y=dot(i_src,yaxis);
		o_dst.z=dot(i_src,zaxis);
	}

	inline vec3 mtx3x3::transformtransposed3x3(const vec3& i_src) const
	{
		return vec3
			(
			dot(i_src,xaxis),
			dot(i_src,yaxis),
			dot(i_src,zaxis)
			);
	}

	inline void mtx3x3::transpose3x3()
	{
		float tmp;
		tmp=_12; _12=_21; _21=tmp;
		tmp=_13; _13=_31; _31=tmp;
		tmp=_23; _23=_32; _32=tmp;
	}

	inline void mtx3x3::transpose3x3(const mtx3x3& i_src)
	{
		_11=i_src._11; _12=i_src._21; _13=i_src._31;
		_21=i_src._12; _22=i_src._22; _23=i_src._32;
		_31=i_src._13; _32=i_src._23; _33=i_src._33;
	}


	inline void mtx3x3::invert3x3(const mtx3x3& i_src)
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
	}

	inline void mtx3x3::abs(const mtx3x3& i_src)
	{
		axisx().abs(i_src.axisx());
		axisy().abs(i_src.axisy());
		axisz().abs(i_src.axisz());

	}

	inline void mtx3x3::multiply3x3(const mtx3x3& i_src1, const mtx3x3& i_src2)
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
	}

	inline void mtx3x3::multiplytransposed3x3(const mtx3x3& i_src1, const mtx3x3& i_src2transposed)
	{
		i_src2transposed.transformtransposed3x3(axisx(),i_src1.axisx());
		i_src2transposed.transformtransposed3x3(axisy(),i_src1.axisy());
		i_src2transposed.transformtransposed3x3(axisz(),i_src1.axisz());
	}
	
	inline void mtx3x3::fromeuler(float i_xangle, float i_yangle, float i_zangle)
	{
		float sx=(sin(i_xangle)), cx=(cos(i_xangle));
		float sy=(sin(i_yangle)), cy=(cos(i_yangle));
		float sz=(sin(i_zangle)), cz=(cos(i_zangle));

		_11 = cz*cy+sz*sx*sy;	_12 = sz*cx;	_13 = sz*sx*cy-cz*sy;
		_21 = cz*sx*sy-sz*cy;	_22 = cz*cx;	_23 = sz*sy+cz*sx*cy;
		_31 = cx*sy;			_32 = -sx;		_33 = cx*cy;
	}

	inline void mtx3x3::geteuler(float& o_xangle, float& o_yangle, float& o_zangle) const
	{
		float a;
		o_xangle = -math::asin(axisz().y);
		a = cos( o_xangle );

		if (math::abs(a)>0.0001f) 
		{
			o_zangle=atan2f(axisx().y, axisy().y);
			o_yangle=atan2f(axisz().x, axisz().z);
		}
		else 
		{
			o_zangle=0.0f;
			o_yangle=atan2f(axisy().x, axisx().x);
		}
	}
	

}// namespace math
#endif//_mtx3x3_h_