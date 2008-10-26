#ifndef _ssevec3_h_
#define _ssevec3_h_

#include <xmmintrin.h>
#include <assert.h>

	_declspec(align (16)) class ssevec3
	{
	public:
		ssevec3()
		{
		}

		ssevec3(float i_X, float i_Y, float i_Z)
		{
			vec=_mm_setr_ps(i_X,i_Y,i_Z,0);
		}

		void set(float i_X, float i_Y, float i_Z)
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

		float operator[](int i) const
		{
			assert((0 <= i) && (i <= 3));
			float *fp = (float*)&vec;
			return *(fp+i);
		}
/*
		float& operator[](int i)
		{
			assert((0 <= i) && (i <= 3));
			float *fp = (float*)&vec;
			return *(fp+i);
		}
*/
/*
		float square_length()
		{
			ssevec3 r = _mm_mul_ps(*this,*this);
			F32vec1 t = _mm_add_ss(_mm_shuffle_ps(r,r,1), _mm_add_ps(_mm_movehl_ps(r,r),r));
			return *(float *)&t;
		}

		float length()
		{
			return Sqrt(SquareLength());
		}
*/
		__m128	vec;
	}; //class ssevec3
#endif//_ssevec3_h_