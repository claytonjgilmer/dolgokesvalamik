#ifndef _indexbuffer_h_
#define _indexbuffer_h_

#include <d3d9.h>

	class indexbuffer
	{
	public:
		indexbuffer(unsigned i_indexnum, int i_32bit=false);
		~indexbuffer();
		void* lock()
		{
			void* ret;
			m_hwbuffer->Lock(0,0,&ret,0);

			return ret;
		}

		void unlock()
		{
			m_hwbuffer->Unlock();
		}
//	private:
		IDirect3DIndexBuffer9* m_hwbuffer;
		const int m_indexsize;
	};
#endif//_indexbuffer_h_