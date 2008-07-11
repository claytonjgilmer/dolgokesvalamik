/*
	betolt modellt, texturat (resourcemanagerbe)
	modellekbol lehet peldanyt kerni (klonozni)

*/
#ifndef _rendersystem_h_
#define _rendersystem_h_

#include "containers/listallocator.h"
#include "renderqueue.h"
#include "threading/taskmanager.h"

#include "rendervertexbuffer.h"
#include "renderindexbuffer.h"
#include "vertexelements.h"

#include "utils/singleton.h"
#include "math/mtx4x4.h"
#include <d3d9.h>
#include <d3dx9.h>

namespace render
{
	class systemdesc
	{
	public:
		bool					m_windowed;
		unsigned				m_backbuffercount;
		unsigned				m_screenwidth;
		unsigned				m_screenheight;
		HWND					m_window;
	};

	class texture;
	class state;

	class system
	{
		DECLARE_SINGLETON_DESC(system,systemdesc);
	public:
		system(const systemdesc*);
		~system();
		vertexbuffer* create_vertexbuffer(unsigned i_vertexnum, const ctr::vector<vertexelements>& i_vertexelements) const;
		void release_vertexbuffer(vertexbuffer* i_vb) const;
		indexbuffer* create_indexbuffer(unsigned i_indexnum) const;
		void release_indexbuffer(indexbuffer* i_ib) const;
		void create_texture(LPDIRECT3DTEXTURE9& o_hwbuf,const void* i_buf, unsigned i_size) const;
		void release_texture(LPDIRECT3DTEXTURE9 i_hwbuf) const;

		void create_shader(LPD3DXEFFECT& i_effect,const void* i_buf, unsigned i_bufsize) const;
		void release_shader(LPD3DXEFFECT i_effect) const;

		void set_renderstate(const state& i_state);

		void render();

		void add_mesh(mesh* i_mesh, const math::mtx4x3& i_mtx, unsigned i_queueindex=0);

		void init_queues(const ctr::vector<ctr::string> i_queuenames);

		void set_projection_params(float i_fov, float i_aspect, float i_nearz, float i_farz, const math::mtx4x4& i_viewmatrix);


		LPDIRECT3DDEVICE9 device() const;
	private:
		LPDIRECT3DDEVICE9 m_device;
		LPDIRECT3D9 m_sys;
		ctr::vector<render::queue> m_queue;
		HWND m_window;

		member(math::mtx4x4,viewprojection_matrix);
	};
}
#endif//_rendersystem_h_