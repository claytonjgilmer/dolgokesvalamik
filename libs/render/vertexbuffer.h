#ifndef _vertexbuffer_h_
#define _vertexbuffer_h_

#include <d3d9.h>
#include "vertexelements.h"
#include "containers/vector.h"

	class vertexbuffer
	{
		friend class rendersystem;
	public:
		vertexbuffer(unsigned i_vertexnum, const vector<vertexelem>& i_vertexelements);
		vertexbuffer(unsigned i_vertexnum, const vector<vertexelem>& i_vertexelements, unsigned i_vertexsize);
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

#ifdef _DEBUG
		const vector<vertexelem> m_vertexelems;
#endif


	};
#endif//_vertexbuffer_h_
