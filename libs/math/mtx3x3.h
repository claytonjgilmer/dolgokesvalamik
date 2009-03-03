#ifndef _mtx3x3_h_
#define _mtx3x3_h_

#include "vec3.h"

	//////////////////////////////////////////////////////////////////////////
	//			mtx3x3
	//////////////////////////////////////////////////////////////////////////

	
	struct mtx3x3;
#ifdef _DEBUG
	void check_mtx3x3(const mtx3x3& m);
#else
#define check_mtx3x3(_M_)
#endif
	struct mtx3x3
	{
		mtx3x3();
		mtx3x3(const vec3& i_x, const vec3& i_y, const vec3& i_z);

		void transpose3x3();
		void transpose3x3(const mtx3x3& i_src);
		void transpose3x3(const mtx3x3* __restrict i_src);

		operator f32* ();
		operator const f32* () const;

		vec3& axis(int i_index);
		const vec3& axis(int i_index) const;

		void identity();
		void clear();

		void from_euler(f32 i_xangle, f32 i_yangle, f32 i_zangle);
		void get_euler(f32& o_xangle, f32& o_yangle, f32& o_zangle) const;

		void transform3x3(vec3& o_dst, const vec3& i_src) const;
		vec3 transform3x3(const vec3& i_src) const;

		void transformtransposed3x3(vec3& o_dst, const vec3& i_src) const;
		vec3 transformtransposed3x3(const vec3& i_src) const;

		void invert3x3(const mtx3x3& i_src);

		void abs(const mtx3x3& i_src);

		void multiply3x3(const mtx3x3& i_src1, const mtx3x3& i_src2);
		void multiply3x3(const mtx3x3* __restrict i_src1, const mtx3x3* __restrict i_src2);
		void multiplytransposed3x3(const mtx3x3& i_src1, const mtx3x3& i_src2transposed);


		union
		{
			struct
			{
				f32	_11,_12,_13;
				f32	_21,_22,_23;
				f32	_31,_32,_33;
			};

			struct
			{
				vec3 x,y,z;
			};
		};
	}; //struct mtx3x3

	inline mtx3x3::mtx3x3()
	{
	}

	inline mtx3x3::mtx3x3(const vec3& i_x, const vec3& i_y, const vec3& i_z)
	{
		x=i_x;
		y=i_y;
		z=i_z;
	}

	inline mtx3x3::operator f32* ()
	{
		return &_11;
	}

	inline mtx3x3::operator const f32* () const
	{
		return &_11;
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

	inline void mtx3x3::clear()
	{
		_11=_22=_33=_12=_13=_21=_23=_31=_32=0.0f;
	}

	inline void mtx3x3::transform3x3(vec3& o_dst, const vec3& i_src) const
	{
		o_dst.x=i_src.x*this->_11+i_src.y*this->_21+i_src.z*this->_31;
		o_dst.y=i_src.x*this->_12+i_src.y*this->_22+i_src.z*this->_32;
		o_dst.z=i_src.x*this->_13+i_src.y*this->_23+i_src.z*this->_33;
		check_vec3(o_dst);
	}

	MLINLINE vec3 mtx3x3::transform3x3(const vec3& i_src) const
	{
	    vec3 t;
	    t.set
			(
			i_src.x*this->_11+i_src.y*this->_21+i_src.z*this->_31,
			i_src.x*this->_12+i_src.y*this->_22+i_src.z*this->_32,
			i_src.x*this->_13+i_src.y*this->_23+i_src.z*this->_33
			);
        return t;
	}

	inline void mtx3x3::transformtransposed3x3(vec3& o_dst, const vec3& i_src) const
	{
		o_dst.x=dot(i_src,this->x);
		o_dst.y=dot(i_src,this->y);
		o_dst.z=dot(i_src,this->z);
	}

	inline vec3 mtx3x3::transformtransposed3x3(const vec3& i_src) const
	{
	    vec3 t; t.set
			(
			dot(i_src,this->x),
			dot(i_src,this->y),
			dot(i_src,this->z)
			);

        return t;
	}

	inline void mtx3x3::transpose3x3()
	{
		f32 tmp;
		tmp=this->_12; this->_12=this->_21; this->_21=tmp;
		tmp=this->_13; this->_13=this->_31; this->_31=tmp;
		tmp=this->_23; this->_23=this->_32; this->_32=tmp;
	}

	__forceinline void mtx3x3::transpose3x3(const mtx3x3& i_src)
	{
		_11=i_src._11; _12=i_src._21; _13=i_src._31;
		_21=i_src._12; _22=i_src._22; _23=i_src._32;
		_31=i_src._13; _32=i_src._23; _33=i_src._33;
	}

	__forceinline void mtx3x3::transpose3x3(const mtx3x3* __restrict i_src)
	{
		_11=i_src->_11; _12=i_src->_21; _13=i_src->_31;
		_21=i_src->_12; _22=i_src->_22; _23=i_src->_32;
		_31=i_src->_13; _32=i_src->_23; _33=i_src->_33;
	}



	inline void mtx3x3::invert3x3(const mtx3x3& i_src)
	{
		f32 d11 = i_src._22*i_src._33 - i_src._23*i_src._32;
		f32 d12 = i_src._23*i_src._31 - i_src._21*i_src._33;
		f32 d13 = i_src._21*i_src._32 - i_src._22*i_src._31;
		f32 det = i_src._11*d11 + i_src._12*d12 + i_src._13*d13;
		f32 id = 1.0f / det;
		f32 id_11 = id * i_src._11;
		f32 id_12 = id * i_src._12;
		f32 id_13 = id * i_src._13;
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
		this->x.abs(i_src.x);
		this->y.abs(i_src.y);
		this->z.abs(i_src.z);

	}

	__forceinline void mtx3x3::multiply3x3(const mtx3x3& i_src1, const mtx3x3& i_src2)
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
		check_mtx3x3(*this);
	}

	__forceinline void mtx3x3::multiply3x3(const mtx3x3* __restrict i_src1, const mtx3x3* __restrict i_src2)
	{
		_11=i_src1->_11*i_src2->_11+i_src1->_12*i_src2->_21+i_src1->_13*i_src2->_31;
		_12=i_src1->_11*i_src2->_12+i_src1->_12*i_src2->_22+i_src1->_13*i_src2->_32;
		_13=i_src1->_11*i_src2->_13+i_src1->_12*i_src2->_23+i_src1->_13*i_src2->_33;

		_21=i_src1->_21*i_src2->_11+i_src1->_22*i_src2->_21+i_src1->_23*i_src2->_31;
		_22=i_src1->_21*i_src2->_12+i_src1->_22*i_src2->_22+i_src1->_23*i_src2->_32;
		_23=i_src1->_21*i_src2->_13+i_src1->_22*i_src2->_23+i_src1->_23*i_src2->_33;

		_31=i_src1->_31*i_src2->_11+i_src1->_32*i_src2->_21+i_src1->_33*i_src2->_31;
		_32=i_src1->_31*i_src2->_12+i_src1->_32*i_src2->_22+i_src1->_33*i_src2->_32;
		_33=i_src1->_31*i_src2->_13+i_src1->_32*i_src2->_23+i_src1->_33*i_src2->_33;
	}

	inline void mtx3x3::multiplytransposed3x3(const mtx3x3& i_src1, const mtx3x3& i_src2transposed)
	{
		i_src2transposed.transformtransposed3x3(this->x,i_src1.x);
		i_src2transposed.transformtransposed3x3(this->y,i_src1.y);
		i_src2transposed.transformtransposed3x3(this->z,i_src1.z);
	}

	inline void mtx3x3::from_euler(f32 i_xangle, f32 i_yangle, f32 i_zangle)
	{
		f32 sx=(sin(i_xangle)), cx=(cos(i_xangle));
		f32 sy=(sin(i_yangle)), cy=(cos(i_yangle));
		f32 sz=(sin(i_zangle)), cz=(cos(i_zangle));

		_11 = cz*cy+sz*sx*sy;	_12 = sz*cx;	_13 = sz*sx*cy-cz*sy;
		_21 = cz*sx*sy-sz*cy;	_22 = cz*cx;	_23 = sz*sy+cz*sx*cy;
		_31 = cx*sy;			_32 = -sx;		_33 = cx*cy;
	}

	inline void mtx3x3::get_euler(f32& o_xangle, f32& o_yangle, f32& o_zangle) const
	{
		f32 a;
		o_xangle = asin(this->z.y);
		a = cos( o_xangle );

		if (fabsf(a)>0.0001f)
		{
			o_zangle=atan2f(this->x.y, this->y.y);
			o_yangle=atan2f(this->z.x, this->z.z);
		}
		else
		{
			o_zangle=0.0f;
			o_yangle=atan2f(this->y.x, this->x.x);
		}
	}
#ifdef _DEBUG
	MLINLINE void check_mtx3x3(const mtx3x3& m)
	{
		check_vec3(m.x);
		check_vec3(m.y);
		check_vec3(m.z);
	}
#endif
#endif//_mtx3x3_h_
