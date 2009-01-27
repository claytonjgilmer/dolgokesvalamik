#ifndef _mtx4x4_h_
#define _mtx4x4_h_

#include "mtx4x3.h"
#include "vec4.h"

	struct mtx4x4
	{
		mtx4x4();
		explicit mtx4x4(const mtx4x3&);

		operator mtx4x3 () const
		{
			mtx4x3 res;
			res._11=_11; res._12=_12; res._13=_13;
			res._21=_21; res._22=_22; res._23=_23;
			res._31=_31; res._32=_32; res._33=_33;
			res._41=_41; res._42=_42; res._43=_43;

			return res;
		}
		void set_projectionmatrix(float i_tgfovhalf, float i_aspect, float i_nearz, float i_farz);
		void multiply(const mtx4x4& i_src1, const mtx4x4& i_src2);
		void multiply_transposed(const mtx4x4& i_src1, const mtx4x4& i_src2t);
		void transform(vec4& o_dst, const vec4& i_src) const;
		void transform_transposed(vec4& o_dst, const vec4& i_src) const;
		void transpose();
		void invert(const mtx4x4& i_src);

		const vec3& axis(int n)
		{
			return (vec3&)m[4*n];
		}

		vec3 column(int n)
		{
			vec3 ret;
			ret.set(m[n],m[n+4],m[n+8]);
			return ret;
		}

		union
		{
			struct
			{
				float _11,_12,_13,_14;
				float _21,_22,_23,_24;
				float _31,_32,_33,_34;
				float _41,_42,_43,_44;
			};

			struct
			{
				vec4 x,y,z,t;
			};

			struct
			{
				float m[16];
			};
		};
	};

	MLINLINE mtx4x4::mtx4x4()
	{
	}

	MLINLINE mtx4x4::mtx4x4(const mtx4x3& i_mtx)
	{
		_11=i_mtx._11;_12=i_mtx._12; _13=i_mtx._13; _14=0;
		_21=i_mtx._21;_22=i_mtx._22; _23=i_mtx._23; _24=0;
		_31=i_mtx._31;_32=i_mtx._32; _33=i_mtx._33; _34=0;
		_41=i_mtx._41;_42=i_mtx._42; _43=i_mtx._43; _44=1;
	}


	MLINLINE void mtx4x4::transform(vec4& o_dst, const vec4& i_src) const
	{
		o_dst.x=_11*i_src.x+_21*i_src.y+_31*i_src.z+_41*i_src.w;
		o_dst.y=_12*i_src.x+_22*i_src.y+_32*i_src.z+_42*i_src.w;
		o_dst.z=_13*i_src.x+_23*i_src.y+_33*i_src.z+_43*i_src.w;
		o_dst.w=_14*i_src.x+_24*i_src.y+_34*i_src.z+_44*i_src.w;
	}

	MLINLINE void mtx4x4::set_projectionmatrix(float i_tgfovhalf, float i_aspect, float i_nearz, float i_farz)
	{
		float    h, w, Q;

		w = 1/(i_tgfovhalf*i_aspect);
		h = 1/i_tgfovhalf;
		Q = i_farz/(i_farz-i_nearz);

		_11 = w; _12=0; _13=0; _14=0;
		_21=0; _22 = h; _23=0; _24=0;
		_31=0; _32=0; _33 = Q; _34=1;
		_41=0; _42=0; _43 = -Q*i_nearz; _44=0;
	}

	MLINLINE void mtx4x4::multiply(const mtx4x4& i_src1, const mtx4x4& i_src2)
	{
		i_src2.transform((vec4&)_11,(const vec4&)i_src1._11);
		i_src2.transform((vec4&)_21,(const vec4&)i_src1._21);
		i_src2.transform((vec4&)_31,(const vec4&)i_src1._31);
		i_src2.transform((vec4&)_41,(const vec4&)i_src1._41);
	}

	MLINLINE void mtx4x4::multiply_transposed(const mtx4x4& i_src1, const mtx4x4& i_src2t)
	{
		i_src2t.transform_transposed(x,i_src1.x);
		i_src2t.transform_transposed(y,i_src1.y);
		i_src2t.transform_transposed(z,i_src1.z);
		i_src2t.transform_transposed(t,i_src1.t);
	}

	MLINLINE void mtx4x4::transform_transposed(vec4& o_dst, const vec4& i_src) const
	{
		o_dst.x=dot4(x,i_src);
		o_dst.y=dot4(y,i_src);
		o_dst.z=dot4(z,i_src);
		o_dst.w=dot4(t,i_src);
	}

	MLINLINE void mtx4x4::transpose()
	{
		swap(_12,_21); swap(_13,_31);swap(_14,_41);
		swap(_23,_32);swap(_24,_42);
		swap(_34,_43);
	}

	MLINLINE void mtx4x4::invert(const mtx4x4& i_src)
	{
		float fA0 = i_src.m[ 0]*i_src.m[ 5] - i_src.m[ 1]*i_src.m[ 4];
		float fA1 = i_src.m[ 0]*i_src.m[ 6] - i_src.m[ 2]*i_src.m[ 4];
		float fA2 = i_src.m[ 0]*i_src.m[ 7] - i_src.m[ 3]*i_src.m[ 4];
		float fA3 = i_src.m[ 1]*i_src.m[ 6] - i_src.m[ 2]*i_src.m[ 5];
		float fA4 = i_src.m[ 1]*i_src.m[ 7] - i_src.m[ 3]*i_src.m[ 5];
		float fA5 = i_src.m[ 2]*i_src.m[ 7] - i_src.m[ 3]*i_src.m[ 6];
		float fB0 = i_src.m[ 8]*i_src.m[13] - i_src.m[ 9]*i_src.m[12];
		float fB1 = i_src.m[ 8]*i_src.m[14] - i_src.m[10]*i_src.m[12];
		float fB2 = i_src.m[ 8]*i_src.m[15] - i_src.m[11]*i_src.m[12];
		float fB3 = i_src.m[ 9]*i_src.m[14] - i_src.m[10]*i_src.m[13];
		float fB4 = i_src.m[ 9]*i_src.m[15] - i_src.m[11]*i_src.m[13];
		float fB5 = i_src.m[10]*i_src.m[15] - i_src.m[11]*i_src.m[14];

		float fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
		m[ 0] =+ i_src.m[ 5]*fB5 - i_src.m[ 6]*fB4 + i_src.m[ 7]*fB3;
		m[ 4] =- i_src.m[ 4]*fB5 + i_src.m[ 6]*fB2 - i_src.m[ 7]*fB1;
		m[ 8] =+ i_src.m[ 4]*fB4 - i_src.m[ 5]*fB2 + i_src.m[ 7]*fB0;
		m[12] =- i_src.m[ 4]*fB3 + i_src.m[ 5]*fB1 - i_src.m[ 6]*fB0;
		m[ 1] =- i_src.m[ 1]*fB5 + i_src.m[ 2]*fB4 - i_src.m[ 3]*fB3;
		m[ 5] =+ i_src.m[ 0]*fB5 - i_src.m[ 2]*fB2 + i_src.m[ 3]*fB1;
		m[ 9] =- i_src.m[ 0]*fB4 + i_src.m[ 1]*fB2 - i_src.m[ 3]*fB0;
		m[13] =+ i_src.m[ 0]*fB3 - i_src.m[ 1]*fB1 + i_src.m[ 2]*fB0;
		m[ 2] =+ i_src.m[13]*fA5 - i_src.m[14]*fA4 + i_src.m[15]*fA3;
		m[ 6] =- i_src.m[12]*fA5 + i_src.m[14]*fA2 - i_src.m[15]*fA1;
		m[10] =+ i_src.m[12]*fA4 - i_src.m[13]*fA2 + i_src.m[15]*fA0;
		m[14] =- i_src.m[12]*fA3 + i_src.m[13]*fA1 - i_src.m[14]*fA0;
		m[ 3] =- i_src.m[ 9]*fA5 + i_src.m[10]*fA4 - i_src.m[11]*fA3;
		m[ 7] =+ i_src.m[ 8]*fA5 - i_src.m[10]*fA2 + i_src.m[11]*fA1;
		m[11] =- i_src.m[ 8]*fA4 + i_src.m[ 9]*fA2 - i_src.m[11]*fA0;
		m[15] =+ i_src.m[ 8]*fA3 - i_src.m[ 9]*fA1 + i_src.m[10]*fA0;

		float fInvDet = ((float)1.0)/fDet;
		m[ 0] *= fInvDet;
		m[ 1] *= fInvDet;
		m[ 2] *= fInvDet;
		m[ 3] *= fInvDet;
		m[ 4] *= fInvDet;
		m[ 5] *= fInvDet;
		m[ 6] *= fInvDet;
		m[ 7] *= fInvDet;
		m[ 8] *= fInvDet;
		m[ 9] *= fInvDet;
		m[10] *= fInvDet;
		m[11] *= fInvDet;
		m[12] *= fInvDet;
		m[13] *= fInvDet;
		m[14] *= fInvDet;
		m[15] *= fInvDet;
	}
#endif//_mtx4x4_h_
