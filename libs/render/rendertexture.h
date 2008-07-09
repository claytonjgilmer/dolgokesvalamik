#ifndef _rendertexture_h_
#define _rendertexture_h_

#include "utils/resource.h"
#include <d3d9.h>

namespace render
{
	class texture:public utils::resource
	{
		friend class utils::referencepointer<texture>;
	public:
		texture* Next;
		texture(const void* i_buf, unsigned i_size, const char* i_name);

		IDirect3DTexture9* m_hwbuffer;
	private:
		~texture();
	};

}
#endif//_rendertexture_h_