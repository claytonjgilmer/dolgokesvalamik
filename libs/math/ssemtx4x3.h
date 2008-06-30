#ifndef _ssemtx4x3_h_
#define _ssemtx4x3_h_

#include "ssevec3.h"

namespace math
{
	_declspec(align (16)) class ssemtx4x3
	{
	public:
		ssevec3 xaxis;
		ssevec3 yaxis;
		ssevec3 zaxis;
		ssevec3 trans;

		ssevec3 transform(const ssevec3& i_src) const;
		void transform(ssevec3& i_dst, const ssevec3& i_src) const;
		void multiply(const ssemtx4x3& i_src1, const ssemtx4x3& i_src2);

		static ssemtx4x3 identitymtx()
		{
			ssemtx4x3 ret;

			ret.xaxis.set(1,0,0);
			ret.yaxis.set(0,1,0);
			ret.zaxis.set(0,0,1);
			ret.trans.set(0,0,0);

			return ret;
		}
	};//class ssemtx4x3

	inline ssevec3 ssemtx4x3::transform(const ssevec3& i_src) const
	{
		return i_src*xaxis+i_src*yaxis+i_src*zaxis+trans;
	}

	inline void ssemtx4x3::transform(ssevec3& i_dst, const ssevec3& i_src) const
	{
		i_dst=i_src*xaxis+i_src*yaxis+i_src*zaxis+trans;
	}
}//namespace math
#endif//_ssemtx4x3_h_