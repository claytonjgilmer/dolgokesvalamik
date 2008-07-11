#ifndef _rendermesh_h_
#define _rendermesh_h_

#include "math/mtx4x3.h"
#include "rendervertexbuffer.h"
#include "renderindexbuffer.h"
#include "rendersubmesh.h"
#include "containers/vector.h"
#include "utils/resource.h"

namespace render
{
	class mesh:public utils::resource
	{
	public:
		mesh(const char* i_name);
		~mesh();
		void render(const math::mtx4x3& i_mtx);

		void set_vertexbuffer(vertexbuffer* i_vb);
		void set_indexbuffer(indexbuffer* i_ib);

		vertexbuffer* m_vb;
		indexbuffer* m_ib;

		ctr::vector<submesh> m_submeshbuf;
	};
}
#endif//_rendermesh_h_