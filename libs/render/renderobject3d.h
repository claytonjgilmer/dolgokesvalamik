#ifndef _renderobject3d_h_
#define _renderobject3d_h_

namespace render
{
	class object3d:public scene::node
	{
	public:
		object3d();
		~object3d();
		void set_mesh(mesh*);
	protected:
		mesh* m_mesh;
	}
}
#endif//_renderobject3d_h_