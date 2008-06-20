#ifndef _rendertexture_h_
#define _rendertexture_h_

#include "..\base\referencedobject.h"

namespace render
{
	class texture:public referencedobject
	{
	public:
	private:
		texture();
		~texture();
		IDirect3DTexture9* m_hwbuffer;
	};

	inline void texture::texture()
	{
		m_hwbuffer=NULL;
	}

	inline void texture::~texture()
	{
	}
}
#endif//_rendertexture_h_