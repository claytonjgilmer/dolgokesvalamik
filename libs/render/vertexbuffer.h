#ifndef _vertexbuffer_h_
#define _vertexbuffer_h_

#include <d3d9.h>
#include "vertexelements.h"
#include "containers/vector.h"

namespace render
{
	class system;
	class vertexbuffer
	{
		friend class system;
	public:
		vertexbuffer(unsigned i_vertexnum, const ctr::vector<vertexelem>& i_vertexelements);
		vertexbuffer(unsigned i_vertexnum, const ctr::vector<vertexelem>& i_vertexelements, unsigned i_vertexsize);
		~vertexbuffer();
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
		LPDIRECT3DVERTEXBUFFER9 m_hwbuffer;
		LPDIRECT3DVERTEXDECLARATION9 m_decl;
		unsigned m_vertexsize;

	};
}
#endif//_vertexbuffer_h_