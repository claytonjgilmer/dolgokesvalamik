#ifndef _renderobject3d_h_
#define _renderobject3d_h_

#include "scene/node.h"
#include "render/rendermesh.h"

namespace render
{
	class object3d:public scene::node
	{
	public:
		object3d();
		~object3d();
		void set_mesh(mesh*);

		object3d* Next;
	protected:
		mesh* m_mesh;
	};
}
#endif//_renderobject3d_h_