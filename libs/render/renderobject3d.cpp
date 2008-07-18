#include "renderobject3d.h"
#include "rendersystem.h"

namespace render
{
	object3d::object3d(const char* i_name):
	scene::node(i_name),
	m_mesh(NULL)
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
			if (ptr->m_mesh)
				system::instance()->add_mesh(ptr->m_mesh,ptr->get_worldposition());

			ptr=(object3d*)ptr->get_next();
		}
	}
	
	
	
}