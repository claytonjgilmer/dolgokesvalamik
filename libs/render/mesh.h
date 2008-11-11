#ifndef _mesh_h_
#define _mesh_h_

#include "math/mtx4x3.h"
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "submesh.h"
#include "containers/vector.h"
#include "utils/resource.h"
#include "utils/auto_ptr.h"

	class mesh:public resource
	{
	public:
		mesh(const char* i_name);

		void set_vertexbuffer(vertexbuffer* i_vb);
		void set_indexbuffer(indexbuffer* i_ib);

		auto_ptr<vertexbuffer> m_vb;
		auto_ptr<indexbuffer> m_ib;

		vector<submesh> m_submeshbuf;
	private:
		~mesh();
	};
#endif//_mesh_h_
