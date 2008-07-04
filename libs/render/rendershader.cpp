#include "rendershader.h"
#include "rendersystem.h"
#include "shadermanager.h"

namespace render
{
	shader::shader(const void* i_buf, unsigned i_size, const char* i_name):
	resource(i_name)
	{
		Next=NULL;
		system::instance()->create_shader(m_effect,i_buf,i_size);
	}

	shader::~shader()
	{
		if (m_effect)
			system::instance()->release_shader(m_effect);

		shadermanager::instance()->erase_shader(this);
	}
}