#include "stdafx.h"

#include <MMSystem.h>

#include "render/rendersystem.h"
#include "file/filesystem.h"
#include "threading/taskmanager.h"
#include "render/shadermanager.h"
#include "render/texturemanager.h"
#include "math/mtx4x4.h"

struct game
{
	
};

threading::taskmanager* g_taskmanager=NULL;

RECT g_rect;
unsigned g_time;
HWND g_hwnd;

void init_app(HWND i_hwnd)
{
	file::system::create();
	file::system::instance()->register_path("shader","shader");
	file::system::instance()->register_path("texture","texture");

	g_taskmanager=new threading::taskmanager(4);

	render::shadermanagerdesc shaderdesc("shader");
	render::shadermanager::create(&shaderdesc);

	render::texturemanagerdesc textdesc("texture");
	render::texturemanager::create(&textdesc);

	render::systemdesc renderdesc;
	renderdesc.m_backbuffercount=2;
	RECT rect;
	GetClientRect(i_hwnd,&rect);
	renderdesc.m_screenwidth=abs(rect.right-rect.left);
	renderdesc.m_screenheight=abs(rect.bottom-rect.top);
	renderdesc.m_window=i_hwnd;
	renderdesc.m_windowed=true;

	render::system::create(&renderdesc);

	::GetWindowRect(i_hwnd,&g_rect);
	g_time=timeGetTime();
	g_hwnd=i_hwnd;

	math::mtx4x4 mtx; mtx.set_projectionmatrix(tan(math::degreetorad(45)),4.0f/3,1,10000);

}

void update_app()
{
	unsigned acttime=::timeGetTime();
	unsigned deltatime=acttime-g_time;

	if (deltatime<33)
		return;

	g_time=acttime;

	
	unsigned width=abs(g_rect.left-g_rect.right);
	unsigned heigth=abs(g_rect.bottom-g_rect.top);

	::MoveWindow(g_hwnd,g_rect.left+(unsigned)(100*sin(acttime/1000.0)),g_rect.top,width,heigth,TRUE);
//	::GetWindowRect()
//		::SetWindowPos()
//		::setwin
//	::MoveWindow()
}

void exit_app()
{
	file::system::release();
	delete g_taskmanager; g_taskmanager=NULL;
	render::shadermanager::release();
	render::texturemanager::release();
	render::system::release();
}