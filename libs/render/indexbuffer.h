#ifndef _indexbuffer_h_
#define _indexbuffer_h_

#include <d3d9.h>

namespace render
{
	class indexbuffer
	{
	public:
		indexbuffer(unsigned i_indexnum);
		~indexbuffer();
		unsigned short* lock()
		{
			void* ret;
			m_hwbuffer->Lock(0,0,&ret,0);

			return (unsigned short*)ret;
		}

		void unlock()
		{
			m_hwbuffer->Unlock();
		}
//	private:
		IDirect3DIndexBuffer9* m_hwbuffer;
	};
}
#endif//_indexbuffer_h_