#include "rendersystem.h"

namespace render
{
	system::system(const systemdesc& i_systemdesc):
	m_taskmanager(i_systemdesc.m_taskmanager)
	{

	}

	system::~system()
	{

	}

	void system::renderqueues()
	{
	}

	vertexbuffer* system::createvertexbuffer(unsigned i_vertexnum)
	{
		vertexbuffer* vb=new vertexbuffer;

		return vb;
	}

	indexbuffer* system::createindexbuffer(unsigned i_indexnum)
	{
		indexbuffer* ib=new indexbuffer;

		return ib;

	}
}