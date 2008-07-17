#include "indexbuffer.h"
#include "rendersystem.h"

namespace render
{
	indexbuffer::indexbuffer(unsigned i_indexnum)
	{
		D3DFORMAT indexformat=D3DFMT_INDEX16;
		const int indexsize=sizeof(short);

		system::instance()->device()->CreateIndexBuffer(	i_indexnum*indexsize,
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