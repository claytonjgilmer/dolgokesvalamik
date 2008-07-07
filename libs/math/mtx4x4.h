#ifndef _mtx4x4_h_
#define _mtx4x4_h_

#include "mtx4x3.h"
#include "vec4.h"

namespace math
{
	struct mtx4x4
	{
		mtx4x4();
		explicit mtx4x4(const mtx4x3&);

		void set_projectionmatrix(float i_tgfovhalf, float i_aspect, float i_nearz, float i_farz);
		void multiply(const mtx4x4& i_src1, const mtx4x4& i_src2);
		void transform(vec4& o_dst, const vec4& i_src) const;

		float _11,_12,_13,_14;
		float _21,_22,_23,_24;
		float _31,_32,_33,_34;
		float _41,_42,_43,_44;
	};

	MLINLINE mtx4x4::mtx4x4()
	{
	}
/*
	MLINLINE mtx4x4::mtx4x4(const mtx4x3& i_mtx)
	{
		_11=i_mtx._11;_12=i_mtx._12; _13
	}
*/

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
		Q = i_nearz/(i_farz-i_nearz);

		ZeroMemory(this, sizeof(mtx4x4));

		_11 = w;
		_22 = h;
		_33 = Q;
		_43 = -Q*i_nearz;
		_34 = 1;
	}

	MLINLINE void mtx4x4::multiply(const mtx4x4& i_src1, const mtx4x4& i_src2)
	{
		i_src2.transform((vec4&)_11,(const vec4&)i_src1._11);
		i_src2.transform((vec4&)_21,(const vec4&)i_src1._21);
		i_src2.transform((vec4&)_31,(const vec4&)i_src1._31);
		i_src2.transform((vec4&)_41,(const vec4&)i_src1._41);
	}
}
#endif//_mtx4x4_h_