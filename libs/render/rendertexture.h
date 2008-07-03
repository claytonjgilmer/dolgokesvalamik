#ifndef _rendertexture_h_
#define _rendertexture_h_

#include "base/resource.h"
#include <d3d9.h>

namespace render
{
	class texture:public base::resource
	{
	public:
		texture* Next;
		texture(void* i_buf, unsigned i_size, const char* i_name);
		~texture();

	private:
		IDirect3DTexture9* m_hwbuffer;
	};

}
#endif//_rendertexture_h_