#include "indexbuffer.h"
#include "rendersystem.h"

namespace render
{
	indexbuffer::indexbuffer(unsigned i_indexnum, bool i_32bit/* =false */):
	m_indexsize(i_32bit ? sizeof(int) : sizeof(short))
	{
		D3DFORMAT indexformat=i_32bit ? D3DFMT_INDEX32 : D3DFMT_INDEX16;

		system::instance()->device()->CreateIndexBuffer(	i_indexnum*m_indexsize,
			D3DUSAGE_WRITEONLY,
			indexformat,
			D3DPOOL_DEFAULT,
			&m_hwbuffer,
			NULL);
	}

	indexbuffer::~indexbuffer()
	{
		if (m_hwbuffer)
			m_hwbuffer->Release();
	}
}