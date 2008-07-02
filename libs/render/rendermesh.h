#ifndef _rendermesh_h_
#define _rendermesh_h_

#include "math/mtx4x3.h"
#include "rendervertexbuffer.h"
#include "renderindexbuffer.h"
#include "rendertriset.h"
#include "containers/vector.h"

namespace render
{
	class mesh
	{
	public:
		mesh();
		~mesh();
		void render(const math::mtx4x3& i_mtx);

		void set_vertexbuffer(vertexbuffer* i_vb);
		void set_indexbuffer(indexbuffer* i_ib);

	private:
		vertexbuffer* m_vb;
		indexbuffer* m_ib;

		ctr::vector<triset> m_triset;
	};
}
#endif//_rendermesh_h_