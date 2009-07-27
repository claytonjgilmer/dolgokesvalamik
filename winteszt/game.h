#ifndef _game_h_
#define _game_h_

#include "input/inputsystem.h"
#include "threading/taskmanager.h"
#include "render/rendersystem.h"
#include "physics/system/physicssystem.h"
#include "render/shadermanager.h"
#include "render/texturemanager.h"
#include "scene/node.h"
#include "utils/timer.h"

struct gamedesc_t
{
	taskmanagerdesc_t taskmanagerdesc;
	rendersystemdesc renderdesc;
	physicssystemdesc physicsdesc;
	inputinitparams inputdesc;
	texturemanagerdesc texturedesc;
	shadermanagerdesc shaderdesc;

	string file_name;
};

struct game_t
{
	void init(const gamedesc_t& gdesc);
	void execute();
	void exit();

	node_t* scene_node;
	timer_t frame_timer;
	float frame_time;
};

extern game_t* g_game;
#endif//_game_h_