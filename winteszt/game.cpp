#include "file/filesystem.h"
#include "render/rendersystem.h"
#include "physics/system/physicssystem.h"
#include "threading/taskmanager.h"
#include "json/jsonparser.h"
#include "utils/performancemeter.h"
#include "scene/sceneloader.h"
#include "render/objectmanager.h"

#include "game.h"

void game_t::init(const gamedesc_t& gdesc)
{
	perf_analizer_t::create();
	filesystem::create();
	filesystem::ptr->register_path("shader","shader\\");
	filesystem::ptr->register_path("texture","texture\\");
	filesystem::ptr->register_path("model","model\\");

	taskmanager_t::create(&gdesc.taskmanagerdesc);

	objectmanager::create(&objectmanagerdesc());
	shadermanager::create(&gdesc.shaderdesc);
	inputsystem::create(&gdesc.inputdesc);
	texturemanager::create(&gdesc.texturedesc);
	physicssystem::create(&gdesc.physicsdesc);
	rendersystem::create(&gdesc.renderdesc);

	scene_node=load_scene(gdesc.file_name.c_str());

	node_t* ptr=scene_node;

	while (ptr)
	{
		ptr->init();
		ptr=ptr->get_next();
	}

	frame_timer.reset();
}

void game_t::exit()
{
	node_t* ptr=scene_node;

	while (ptr)
	{
		ptr->exit();
		ptr=ptr->get_next();
	}



	rendersystem::release();
	physicssystem::release();
	texturemanager::release();
	inputsystem::release();
	shadermanager::release();
	taskmanager_t::release();
	filesystem::release();
	objectmanager::release();
	perf_analizer_t::release();
}

void game_t::execute()
{
	perf_analizer_t::ptr->flush();
	taskmanager_t::ptr->flush();
	frame_timer.stop();
	frame_time=frame_timer.get_seconds();
	frame_timer.reset();

	static float sum_time=0;
	sum_time+=frame_time;

	if (sum_time<.033f)
	{
		inputsystem::ptr->update(false);
	}
	else
	{
		inputsystem::ptr->update(true);
		sum_time-=.033f;
	}
	node_t* ptr=scene_node;

	while (ptr)
	{
		ptr->execute();
		ptr=ptr->get_next();
	}

	static bool simpause=false;
	static bool simstep=false;

	if (inputsystem::ptr->KeyPressed(KEYCODE_P) && !simstep)
		simpause=!simpause;
	if (inputsystem::ptr->KeyPressed(KEYCODE_P) && simstep)
		simpause=false;
	if (inputsystem::ptr->KeyPressed(KEYCODE_O))
	{
		simstep=!simstep;
		simpause=false;
	}

	if (!simpause)
	physicssystem::ptr->simulate(min(frame_time,.033f));

	if (simstep)
		simpause=true;

	ptr=scene_node;
	while (ptr)
	{
		ptr->render();
		ptr=ptr->get_next();
	}

	rendersystem::ptr->render();

}
