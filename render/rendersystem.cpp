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
}