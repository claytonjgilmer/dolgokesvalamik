#include "renderobject3d.h"

namespace render
{
	object3d::object3d():
	m_mesh(NULL),
	{
	}
	
	object3d::~object3d()
	{
		if (m_mesh)
			delete m_mesh;
	}
	
	
	
}