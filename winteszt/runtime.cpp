#include "stdafx.h"
#include "render/cameranode.h"
#include "game.h"

game_t* g_game;

void init_app(HWND hwnd)
{
	camera_node_t cm;
	g_game=new game_t;

	gamedesc_t gd;
	gd.file_name="teszt.json";

	gd.inputdesc.m_Window=hwnd;

	gd.physicsdesc.gravity.set(0,-10,0);
	gd.physicsdesc.maxsubstepcount=10;
	gd.physicsdesc.parallel_processing=0;
	gd.physicsdesc.solver_lambda_relaxation=1;
	gd.physicsdesc.solver_max_penetration=0.01f;
	gd.physicsdesc.solver_positioncorrection_rate=0.1f;
	gd.physicsdesc.timestep=.01f;

	gd.renderdesc.m_window=hwnd;
	gd.renderdesc.m_windowed=true;
	gd.renderdesc.m_clear_color.set(128,128,128,255);
	gd.renderdesc.m_backbuffercount=2;
	RECT rect;
	GetClientRect(hwnd,&rect);
	gd.renderdesc.m_screenwidth=abs(rect.right-rect.left);
	gd.renderdesc.m_screenheight=abs(rect.bottom-rect.top);

	gd.taskmanagerdesc.m_threadnum=atol(getenv("NUMBER_OF_PROCESSORS"))-1;

	g_game->init(gd);
}

void exit_app()
{
	g_game->exit();
	delete g_game;
}

void update_app()
{
	g_game->execute();
}

void change_screen_size(unsigned i_newwidth, unsigned i_newheight)
{
	if (!i_newheight || !i_newwidth)
		return;

	if (g_game)
		rendersystem::ptr->set_aspect((f32)i_newwidth/(f32)i_newheight);
}

void reload_shaders()
{
	//	if (g_game->inited)
	//		shadermanager::ptr->reload_shaders();
}

