#ifndef _mesh_h_
#define _mesh_h_

#include "math/mtx4x3.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "submesh.h"
#include "containers/vector.h"
#include "utils/resource.h"
#include "utils/auto_ptr.h"

namespace render
{
	class mesh:public utils::resource
	{
	public:
		mesh(const char* i_name);

		void set_vertexbuffer(vertexbuffer* i_vb);
		void set_indexbuffer(indexbuffer* i_ib);

		utils::auto_ptr<vertexbuffer> m_vb;
		utils::auto_ptr<indexbuffer> m_ib;

		ctr::vector<submesh> m_submeshbuf;
	private:
		~mesh();
	};
}
#endif//_mesh_h_