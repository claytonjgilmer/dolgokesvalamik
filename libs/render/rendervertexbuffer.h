#ifndef _rendervertexbuffer_h_
#define _rendervertexbuffer_h_

#include <d3d9.h>

namespace render
{
	class system;
	class vertexbuffer
	{
		friend class system;
	public:
	private:
		vertexbuffer(){}
		~vertexbuffer(){}
		IDirect3DVertexBuffer9* m_hwbuffer;
	};
}
#endif//_rendervertexbuffer_h_