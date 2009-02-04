#include "mesh.h"
#include "rendersystem.h"

	mesh_t::mesh_t(const char* i_name):resource(i_name)
	{
	}

	mesh_t::~mesh_t()
	{
	}

	void mesh_t::set_indexbuffer(indexbuffer* i_ib)
	{
		m_ib=i_ib;
	}

	void mesh_t::set_vertexbuffer(vertexbuffer* i_vb)
	{
		m_vb=i_vb;
	}
