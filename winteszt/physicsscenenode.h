#ifndef _physicsscenenode_h_
#define _physicsscenenode_h_

#include "scene/node.h"
struct physics_scene_node:node_t
{
	DECLARE_OBJECT(physics_scene_node);
	physics_scene_node();
	void init();

	//props
	vec3 gravity;
};
#endif//_physicsscenenode_h_