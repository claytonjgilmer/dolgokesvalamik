#include "rendermesh.h"
#include "rendersystem.h"

namespace render
{
	mesh::mesh()
	{
		m_vb=NULL;
		m_ib=NULL;
	}

	mesh::~mesh()
	{
		system::instance()->release_vertexbuffer(m_vb);
		system::instance()->release_indexbuffer(m_ib);
	}

	void mesh::set_indexbuffer(indexbuffer* i_ib)
	{
		system::instance()->release_indexbuffer(m_ib);
		m_ib=i_ib;
	}

	void mesh::set_vertexbuffer(vertexbuffer* i_vb)
	{
		system::instance()->release_vertexbuffer(m_vb);
		m_vb=i_vb;
	}
}