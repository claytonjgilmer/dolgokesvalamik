#include "texture.h"
#include "rendersystem.h"
#include "texturemanager.h"

	texture::texture(const void* i_buf, unsigned i_size, const char* i_name):
		resource(i_name)
	{
		Next=NULL;
		D3DXCreateTextureFromFileInMemory(rendersystem::ptr->device(),i_buf,i_size,&m_hwbuffer);
	}

	texture::~texture()
	{
		if (m_hwbuffer)
			m_hwbuffer->Release();

		texturemanager::ptr->erase_texture(this);
	}
