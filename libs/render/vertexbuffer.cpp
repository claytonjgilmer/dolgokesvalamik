#include "vertexbuffer.h"
#include "rendersystem.h"




namespace render
{

	D3DVERTEXELEMENT9 g_decl[vertexelement_num+1]=
	{
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};

	vertexbuffer::vertexbuffer(unsigned i_vertexnum, const ctr::vector<vertexelem>& i_vertexelements)
#ifdef _DEBUG
		:m_vertexelems(i_vertexelements)
#endif
	{
		for (int n=0; n<vertexelement_num; ++n)
			g_decl[n].UsageIndex=0;

		unsigned siz=0;

		ctr::fixedvector<D3DVERTEXELEMENT9,20> decl;

		for (unsigned n=0; n<i_vertexelements.size(); ++n)
		{
			if (i_vertexelements[n].m_elemtype!=vertexelement_unused)
			{
				decl.push_back(g_decl[i_vertexelements[n].m_elemtype]);
				++g_decl[i_vertexelements[n].m_elemtype].UsageIndex;
				decl.back().Offset=siz;
			}
			siz+=i_vertexelements[n].m_elemsize*4;
		}

		decl.push_back(g_decl[vertexelement_num]);

		system::instance()->device()->CreateVertexBuffer(i_vertexnum*siz,D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&m_hwbuffer,NULL);
		system::instance()->device()->CreateVertexDeclaration(&decl[0],&m_decl);
		m_vertexsize=siz;
	}

	vertexbuffer::vertexbuffer(unsigned i_vertexnum, const ctr::vector<vertexelem>& i_vertexelements,unsigned i_vertexsize)
#ifdef _DEBUG
		:m_vertexelems(i_vertexelements)
#endif
	{
		for (int n=0; n<vertexelement_num; ++n)
			g_decl[n].UsageIndex=0;

		unsigned siz=0;

		ctr::fixedvector<D3DVERTEXELEMENT9,20> decl;

		for (unsigned n=0; n<i_vertexelements.size(); ++n)
		{
			if (i_vertexelements[n].m_elemtype!=vertexelement_unused)
			{
				decl.push_back(g_decl[i_vertexelements[n].m_elemtype]);
				++g_decl[i_vertexelements[n].m_elemtype].UsageIndex;
				decl.back().Offset=siz;
			}
			siz+=i_vertexelements[n].m_elemsize*4;
		}

		decl.push_back(g_decl[vertexelement_num]);

		system::instance()->device()->CreateVertexBuffer(i_vertexnum*i_vertexsize,D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&m_hwbuffer,NULL);
		system::instance()->device()->CreateVertexDeclaration(&decl[0],&m_decl);
		m_vertexsize=i_vertexsize;
	}

	vertexbuffer::~vertexbuffer()
	{
		m_hwbuffer->Release();
		m_decl->Release();
	}
}