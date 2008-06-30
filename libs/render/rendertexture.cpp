#include "rendertexture.h"
#include "rendersystem.h"
#include "texturemanager.h"

namespace render
{
	texture::texture(void* i_buf, unsigned i_size, const char* i_name):
	m_name(i_name),
	Name(m_name)
	{
		Next=NULL;
		system::instance()->create_texture(m_hwbuffer,i_buf,i_size);
	}

	texture::~texture()
	{
		if (m_hwbuffer)
			system::instance()->release_texture(m_hwbuffer);

		texturemanager::instance()->erase_texture(this);
	}
}