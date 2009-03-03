#ifndef _ssevec3_h_
#define _ssevec3_h_

#include <xmmintrin.h>
#include <assert.h>

	_declspec(align (16)) struct ssevec3
	{
		ssevec3()
		{
		}

		ssevec3(f32 i_X, f32 i_Y, f32 i_Z)
		{
			vec=_mm_setr_ps(i_X,i_Y,i_Z,0);
		}

		void set(f32 i_X, f32 i_Y, f32 i_Z)
		{
			vec=_mm_setr_ps(i_X,i_Y,i_Z,0);
		}

		ssevec3(const ssevec3& i_Other)
		{
			vec=i_Other.vec;
		}

		operator  __m128() const	
		{
			return vec;
		}

		ssevec3(__m128 m)					
		{
			vec = m;
		}

		void operator =(const ssevec3& i_Other)
		{
			vec=i_Other.vec;
		}

		ssevec3 operator+(const ssevec3& i_Other) const
		{
			return _mm_add_ps(vec,i_Other.vec);
		}

		ssevec3 operator-(const ssevec3& i_Other) const
		{
			return _mm_sub_ps(vec,i_Other.vec);
		}

		ssevec3 operator*(const ssevec3& i_Other) const
		{
			return _mm_mul_ps(vec,i_Other.vec);
		}

		void operator +=(const ssevec3& i_Other)
		{
			vec=_mm_add_ps(vec,i_Other.vec);
		}

		void operator -=(const ssevec3& i_Other)
		{
			vec=_mm_sub_ps(vec,i_Other.vec);
		}

		f32 operator[](int i) const
		{
			assert((0 <= i) && (i <= 3));
			f32 *fp = (f32*)&vec;
			return *(fp+i);
		}
/*
		f32& operator[](int i)
		{
			assert((0 <= i) && (i <= 3));
			f32 *fp = (f32*)&vec;
			return *(fp+i);
		}
*/
/*
		f32 square_length()
		{
			ssevec3 r = _mm_mul_ps(*this,*this);
			F32vec1 t = _mm_add_ss(_mm_shuffle_ps(r,r,1), _mm_add_ps(_mm_movehl_ps(r,r),r));
			return *(f32 *)&t;
		}

		f32 length()
		{
			return Sqrt(SquareLength());
		}
*/
		__m128	vec;
	}; //ssevec3
#endif//_ssevec3_h_