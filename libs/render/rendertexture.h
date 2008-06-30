#ifndef _rendertexture_h_
#define _rendertexture_h_

#include "base/referencedobject.h"
#include "containers/string.h"
#include <d3d9.h>

namespace render
{
	class system;
	class texturemanager;

	class texture:public base::referencedobject
	{
		friend class system;
		friend class texturemanager;
	public:
		texture* Next;
		const ctr::string& Name;

	private:
		texture(void* i_buf, unsigned i_size, const char* i_name);
		~texture();
		IDirect3DTexture9* m_hwbuffer;

		ctr::string m_name;
	};

}
#endif//_rendertexture_h_