#include "mesh.h"
#include "rendersystem.h"

namespace render
{
	mesh::mesh(const char* i_name):resource(i_name)
	{
	}

	mesh::~mesh()
	{
	}

	void mesh::set_indexbuffer(indexbuffer* i_ib)
	{
		m_ib=i_ib;
	}

	void mesh::set_vertexbuffer(vertexbuffer* i_vb)
	{
		m_vb=i_vb;
	}
}