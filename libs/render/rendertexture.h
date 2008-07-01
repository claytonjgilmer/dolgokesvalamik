#ifndef _rendertexture_h_
#define _rendertexture_h_

#include "base/referencedobject.h"
#include "containers/string.h"
#include <d3d9.h>

namespace render
{
	class texture:public base::referencedobject
	{
	public:
		texture* Next;
		const ctr::string& Name;
		texture(void* i_buf, unsigned i_size, const char* i_name);

	private:
		~texture();
		IDirect3DTexture9* m_hwbuffer;

		ctr::string m_name;
	};

}
#endif//_rendertexture_h_