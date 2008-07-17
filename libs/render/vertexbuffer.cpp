#include "vertexbuffer.h"
#include "rendersystem.h"




namespace render
{
	const D3DVERTEXELEMENT9 g_decl[vertexelement_num+1]=
	{
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	vertexbuffer::vertexbuffer(unsigned i_vertexnum, const ctr::vector<vertexelements>& i_vertexelements)
	{
		unsigned siz=0;

		ctr::fixedvector<D3DVERTEXELEMENT9,20> decl;

		for (unsigned n=0; n<i_vertexelements.size(); ++n)
		{
			decl.push_back(g_decl[i_vertexelements[n]]);
			unsigned offs=decl.back().Offset;
			decl.back().Offset=siz;
			siz+=offs;
		}

		decl.push_back(g_decl[vertexelement_num]);

		system::instance()->device()->CreateVertexBuffer(i_vertexnum*siz,D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&m_hwbuffer,NULL);
		system::instance()->device()->CreateVertexDeclaration(&decl[0],&m_decl);
		m_vertexsize=siz;
	}

	vertexbuffer::~vertexbuffer()
	{
		m_hwbuffer->Release();
		m_decl->Release();
	}
}