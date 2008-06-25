#include "rendersystem.h"
#include "base/assert.h"

namespace render
{
	system* g_system=NULL;

	system::system(const systemdesc& i_systemdesc):
	m_taskmanager(i_systemdesc.m_taskmanager)
	{
		base::assertion(g_system!=NULL,"mar volt ilyen");
		g_system=this;
	}

	system::~system()
	{
		g_system=NULL;
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