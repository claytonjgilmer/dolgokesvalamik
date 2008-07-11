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
		void* lock()
		{
			void* ret;
			m_hwbuffer->Lock( 0, sizeof(m_vertexsize), &ret, 0 );
			return ret;
		}
		void unlock()
		{
			m_hwbuffer->Unlock();
		}
	private:
		vertexbuffer(){}
		~vertexbuffer(){}
		LPDIRECT3DVERTEXBUFFER9 m_hwbuffer;
		LPDIRECT3DVERTEXDECLARATION9 m_decl;
		unsigned m_vertexsize;

	};
}
#endif//_rendervertexbuffer_h_