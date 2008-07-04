#ifndef _rendertexture_h_
#define _rendertexture_h_

#include "base/resource.h"
#include <d3d9.h>

namespace render
{
	class texture:public base::resource
	{
		friend class base::referencepointer<texture>;
	public:
		texture* Next;
		texture(const void* i_buf, unsigned i_size, const char* i_name);

	private:
		IDirect3DTexture9* m_hwbuffer;
		~texture();
	};

}
#endif//_rendertexture_h_