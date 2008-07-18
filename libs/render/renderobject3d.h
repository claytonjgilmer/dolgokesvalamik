#ifndef _renderobject3d_h_
#define _renderobject3d_h_

#include "scene/node.h"
#include "render/mesh.h"
#include "utils/auto_ptr.h"

namespace render
{
	class object3d:public scene::node
	{
	public:
		object3d(const char* i_name);
		~object3d();
		void set_mesh(mesh* i_mesh){m_mesh=i_mesh;}
		mesh* get_mesh(){return m_mesh;}

		void render();

		object3d* Next;
	protected:
		utils::auto_ptr<mesh> m_mesh;
	};
}
#endif//_renderobject3d_h_