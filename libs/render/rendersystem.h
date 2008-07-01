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

#include "base/singleton.h"
#include <d3d9.h>

namespace render
{
	class systemdesc
	{
	public:
		threading::taskmanager* m_taskmanager;
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
		DECLARE_SINGLETON(system);
	public:
		vertexbuffer* create_vertexbuffer(unsigned i_vertexnum, unsigned i_vertexsize);
		indexbuffer* create_indexbuffer(unsigned i_indexnum);
		void create_texture(LPDIRECT3DTEXTURE9& o_hwbuf,void* i_buf, unsigned i_size);
		void release_texture(LPDIRECT3DTEXTURE9 i_hwbuf);

		void set_renderstate(const state& i_state);


		void render_queues();
		LPDIRECT3DDEVICE9 device() const;
	private:
		LPDIRECT3DDEVICE9 m_device;
		LPDIRECT3D9 m_sys;
		threading::taskmanager* m_taskmanager;
		ctr::listallocator<queue> m_queues;
		HWND m_window;
	};
}
#endif//_rendersystem_h_