#include "indexbuffer.h"
#include "rendersystem.h"

indexbuffer::indexbuffer(unsigned i_indexnum, int i_32bit/* =0 */):
m_indexsize(i_32bit ? sizeof(int) : sizeof(short)),m_indexcount(i_indexnum)
{
	D3DFORMAT indexformat=i_32bit ? D3DFMT_INDEX32 : D3DFMT_INDEX16;

	rendersystem::ptr->device()->CreateIndexBuffer(	i_indexnum*m_indexsize,
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
