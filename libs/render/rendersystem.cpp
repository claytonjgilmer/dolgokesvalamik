#include <d3d9.h>
#include <d3dx9.h>

#include "rendersystem.h"
#include "utils/assert.h"
#include "renderstate.h"
#include "submesh.h"
#include "texture.h"
#include "shader.h"
#include "math/mtx4x4.h"

	DEFINE_SINGLETON(rendersystem);

	rendersystem::rendersystem(const rendersystemdesc* i_desc)
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
		this->m_window=i_desc->m_window;

		d3dpp.EnableAutoDepthStencil=true;
		d3dpp.AutoDepthStencilFormat=D3DFMT_D24S8;

		d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;

		this->m_sys->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->m_window,
			D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED,
			&d3dpp, &this->m_device);

		if (!this->m_device)
			m_sys->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &this->m_device );

		this->m_device->CreateVertexBuffer( LINENUM_MAX*sizeof(line_struct),
			D3DUSAGE_WRITEONLY,
			D3DFVF_XYZ | D3DFVF_DIFFUSE,
			D3DPOOL_DEFAULT,
			&this->line_buffer,
			NULL );

		D3DXCreateFont( this->m_device, 16, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
			"Times New Roman", &this->font );
		D3DXCreateSprite( this->m_device, &this->font_sprite);

		this->draw_lines=FALSE;
		this->draw_texts=TRUE;


		this->m_queue.resize(1);
		this->m_device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	}

	rendersystem::~rendersystem()
	{
		if (m_device)
			m_device->Release();

		if (m_sys)
			m_sys->Release();

		if (line_buffer)
			line_buffer->Release();
	}

	LPDIRECT3DDEVICE9 rendersystem::device() const
	{
//		assertion(m_device,"rendersystem nincs inicializalva");
		return m_device;
	}

	class rendertask:public task_t
	{
	public:
		void run()
		{
			rendersystem::ptr()->flush_queues();
		}
	};


	void rendersystem::render()
	{
		taskmanager::ptr()->spawn_task(new rendertask);

	}

	void rendersystem::flush_queues()
	{
		m_device->BeginScene();
		m_device->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(23,65,96),1,0);


		for (unsigned int queueindex = 0; queueindex < m_queue.size() ; queueindex++)
		{
			renderqueue& q=m_queue[queueindex];
			const unsigned meshnum=q.m_buf.size();
			const queueelem* buf=q.m_buf;

			for (unsigned meshindex=0; meshindex<meshnum; ++meshindex)
			{
				mesh* m=buf[meshindex].m_mesh;
				m_device->SetStreamSource(0,m->m_vb->m_hwbuffer,0,m->m_vb->m_vertexsize);
				m_device->SetVertexDeclaration(m->m_vb->m_decl);
				m_device->SetIndices(m->m_ib->m_hwbuffer);
				const mtx4x4 objmtx=(mtx4x4)buf[meshindex].m_mtx;
				mtx4x4 faszommtx; faszommtx.multiply(objmtx,m_viewprojection_matrix);

				faszommtx.transpose();

				for (unsigned trisetindex=0; trisetindex<m->m_submeshbuf.size(); ++trisetindex)
				{
					submesh& sm=m->m_submeshbuf[trisetindex];

					for (unsigned txtindex=0; txtindex<sm.m_texturebuf.size(); ++txtindex)
					{
						m_device->SetTexture(txtindex,sm.m_texturebuf[txtindex].get() ? sm.m_texturebuf[txtindex].get()->m_hwbuffer : NULL);
					}


					{
						LPD3DXEFFECT acteffect=sm.m_shader->m_effect;
						acteffect->SetTechnique( "Technique0" );
						acteffect->SetValue("worldViewProj",&faszommtx,sizeof(mtx4x4));
						sm.set_constants();
						unsigned numpasses;
						acteffect->Begin( &numpasses, D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESHADERSTATE);

						for (unsigned uPass = 0; uPass < numpasses; ++uPass )
						{
							acteffect->BeginPass( uPass );
							m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,sm.m_firstvertex,sm.m_numvertices,sm.m_firstindex,sm.m_numindices/3);
							acteffect->EndPass();
						}

						acteffect->End();
					}
				}
			}
			q.m_buf.clear();
		}
		//csikok
		if (this->lines.size())
		{
			void* buf;
			line_buffer->Lock(0,this->lines.size()*sizeof(line_struct),&buf,0);
			memcpy(buf,this->lines,this->lines.size()*sizeof(line_struct));
			this->line_buffer->Unlock();

			D3DMATERIAL9 mat;
			mat.Ambient.r=1;
			mat.Ambient.g=1;
			mat.Ambient.b=1;
			mat.Ambient.a=1;

			mat.Diffuse.r=0;
			mat.Diffuse.g=0;
			mat.Diffuse.b=0;
			mat.Diffuse.a=0;

			mat.Emissive.r=0;
			mat.Emissive.g=0;
			mat.Emissive.b=0;
			mat.Emissive.a=0;

			mat.Power=1.0f;

			mat.Specular.r=0;
			mat.Specular.g=0;
			mat.Specular.b=0;
			mat.Specular.a=0;


			m_device->SetMaterial(&mat);
			m_device->SetTexture(0,NULL);
			m_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
			//		m_RS.m_FVF=-1;

			m_device->SetIndices(NULL);
			m_device->SetStreamSource(0,line_buffer,0, sizeof(vec3)+sizeof(color_r8g8b8a8));

			D3DXMATRIX worldMatrix;
			D3DXMatrixIdentity( &worldMatrix );
			m_device->SetTransform(D3DTS_WORLD,&worldMatrix);
			m_device->SetTransform(D3DTS_PROJECTION,(D3DMATRIX*)&m_projection_matrix);
			m_device->SetTransform(D3DTS_VIEW,(D3DMATRIX*)&m_view_matrix);
			//			m_device->LightEnable( 0, FALSE );

			m_device->SetVertexShader(NULL);
			m_device->SetPixelShader(NULL);

			m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
			m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

			//			m_device->SetRenderState( D3DRS_LIGHTING, FALSE );
			m_device->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);

			m_device->DrawPrimitive(D3DPT_LINELIST,0,lines.size());

			m_device->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
			m_device->SetRenderState( D3DRS_AMBIENT, 0x00000000 );
			//			m_device->SetRenderState( D3DRS_LIGHTING, TRUE );

			//			m_device->LightEnable( 0, TRUE );
			//			m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			//			m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

			lines.clear();
		}

		//szovegek

		if (strings.size())
		{
			this->font_sprite->Begin(D3DXSPRITE_ALPHABLEND);
			for (unsigned n=0; n<this->strings.size();++n)
			{
				RECT rc;
				SetRect( &rc, this->strings[n].x, this->strings[n].y, 0, 0 );
				this->font->DrawText( this->font_sprite, this->strings[n].str.c_str(), -1, &rc, DT_NOCLIP, (D3DXCOLOR&)this->strings[n].color);
			}
			this->font_sprite->End();
			this->strings.clear();
		}




		m_device->EndScene();
		m_device->Present( NULL, NULL, NULL, NULL );
	}

	void rendersystem::add_mesh(mesh* i_mesh, const mtx4x3& i_mtx, unsigned i_queueindex/* =0 */)
	{
		m_queue[i_queueindex].m_buf.push_back(queueelem(i_mesh,i_mtx));
	}

	void rendersystem::set_renderstate(const state& i_state)
	{
		m_device->SetRenderState(i_state.m_type,i_state.m_value);
	}

	void rendersystem::init_queues(const vector<string> i_queuenames)
	{
		m_queue.resize(i_queuenames.size());
		for (unsigned n = 0; n < m_queue.size() ; n++)
			m_queue[n].m_name=i_queuenames[n];
	}

	void rendersystem::set_projection_params(float i_fov, float i_aspect, float i_nearz, float i_farz, const mtx4x4& i_viewmatrix)
	{
		m_view_matrix=i_viewmatrix;
		m_projection_matrix.set_projectionmatrix(tan(i_fov/2), i_aspect,i_nearz,i_farz);
		m_viewprojection_matrix.multiply(i_viewmatrix,m_projection_matrix);
	}
