#include <d3d9.h>
#include <d3dx9.h>

#include "rendersystem.h"
#include "base/assert.h"
#include "renderstate.h"

namespace render
{
	DEFINE_SINGLETON(system);

	system::system(const systemdesc* i_desc):
	m_taskmanager(i_desc->m_taskmanager)
	{
		m_sys = Direct3DCreate9( D3D_SDK_VERSION );

		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory( &d3dpp, sizeof(d3dpp) );

		d3dpp.Windowed=i_desc->m_windowed;
		d3dpp.SwapEffect= !i_desc->m_windowed ? D3DSWAPEFFECT_FLIP : D3DSWAPEFFECT_COPY;

		d3dpp.BackBufferCount=!i_desc->m_windowed ? i_desc->m_backbuffercount : 1;
		d3dpp.BackBufferWidth=i_desc->m_screenwidth;
		d3dpp.BackBufferHeight=i_desc->m_screenheight;
		d3dpp.BackBufferFormat=D3DFMT_A8R8G8B8;

		d3dpp.hDeviceWindow=i_desc->m_window;
		m_window=i_desc->m_window;

		d3dpp.EnableAutoDepthStencil=true;
		d3dpp.AutoDepthStencilFormat=D3DFMT_D24S8;

		d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;

		m_sys->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp, &m_device);

		if (!m_device)
			m_sys->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &m_device );
	}

	system::~system()
	{
		if (m_device)
			m_device->Release();

		if (m_sys)
			m_sys->Release();
	}

	LPDIRECT3DDEVICE9 system::device() const
	{
		return m_device;
	}

	void system::render_queues()
	{
	}

	const D3DVERTEXELEMENT9 g_decl[element_last+1]=
	{
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};

	vertexbuffer* system::create_vertexbuffer(unsigned i_vertexnum, const ctr::vector<vertexelements>& i_vertexelements) const
	{
		vertexbuffer* vb=new vertexbuffer;

		
		unsigned siz=0;

		ctr::fixedvector<D3DVERTEXELEMENT9,20> decl;

		for (unsigned n=0; n<i_vertexelements.size(); ++n)
		{
			decl.push_back(g_decl[i_vertexelements[n]]);
			unsigned offs=decl.back().Offset;
			decl.back().Offset=siz;
			siz+=offs;
		}

		decl.push_back(g_decl[element_last]);

		m_device->CreateVertexBuffer(i_vertexnum*siz,D3DUSAGE_WRITEONLY,0,D3DPOOL_DEFAULT,&vb->m_hwbuffer,NULL);
		m_device->CreateVertexDeclaration(&decl[0],&vb->m_decl);
		return vb;
	}

	void system::release_vertexbuffer(vertexbuffer* i_vb) const
	{
		if (i_vb && i_vb->m_hwbuffer)
			i_vb->m_hwbuffer->Release();

		i_vb->m_hwbuffer=NULL;
	}

	indexbuffer* system::create_indexbuffer(unsigned i_indexnum) const
	{
		indexbuffer* ib=new indexbuffer;
		IDirect3DIndexBuffer9* hwbuf;
		D3DFORMAT indexformat=D3DFMT_INDEX16;
		const int indexsize=sizeof(short);

		m_device->CreateIndexBuffer(	i_indexnum*indexsize,
			D3DUSAGE_WRITEONLY,
			indexformat,
			D3DPOOL_DEFAULT,
			&hwbuf,
			NULL);

		ib->m_hwbuffer=hwbuf;
		return ib;
	}

	void system::release_indexbuffer(indexbuffer* i_ib) const
	{
		if (i_ib && i_ib->m_hwbuffer)
			i_ib->m_hwbuffer->Release();

		i_ib->m_hwbuffer=0;
	}

	void system::create_texture(LPDIRECT3DTEXTURE9& o_hwbuf,const void* i_buf, unsigned i_size) const
	{
		D3DXCreateTextureFromFileInMemory(m_device,i_buf,i_size,&o_hwbuf);
	}

	void system::release_texture(LPDIRECT3DTEXTURE9 i_hwbuf) const
	{
		i_hwbuf->Release();
	}

	void system::create_shader(LPD3DXEFFECT& i_effect,const void* i_buf, unsigned i_bufsize) const
	{
		D3DXCreateEffect(m_device,i_buf,i_bufsize,NULL,NULL,0,NULL,&i_effect,NULL);
	}

	void system::release_shader(LPD3DXEFFECT i_effect) const
	{
		i_effect->Release();
	}

	void system::set_renderstate(const state& i_state)
	{
		m_device->SetRenderState(i_state.m_type,i_state.m_value);
	}
}