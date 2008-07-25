#include "renderobject3d.h"
#include "rendersystem.h"

namespace render
{
	object3d::object3d(const char* i_name):
	scene::node(i_name)
	{
	}
	
	object3d::~object3d()
	{
	}

	void object3d::render()
	{
		object3d* ptr=this;

		while (ptr)
		{
			for (unsigned n=0; n<ptr->m_mesh.size(); ++n)
				system::instance()->add_mesh(ptr->m_mesh[n].get(),ptr->get_worldposition());

			ptr=(object3d*)ptr->get_next();
		}
	}
	
	
	
}