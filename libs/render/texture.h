#ifndef _texture_h_
#define _texture_h_

#include "utils/resource.h"
#include <d3d9.h>

	struct texture:resource
	{
		texture* Next;
		texture(const void* i_buf, unsigned i_size, const char* i_name);

		IDirect3DTexture9* m_hwbuffer;

		~texture();
	};
#endif//_texture_h_
