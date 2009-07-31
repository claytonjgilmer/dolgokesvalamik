#include "physicsscenenode.h"
#include "physics/system/physicssystem.h"

DEFINE_OBJECT(physics_scene_node,node_t);
BIND_PROPERTY(physics_scene_node,gravity,vec3);

physics_scene_node::physics_scene_node()
{
	gravity.set(0,-10,0);
}

void physics_scene_node::init()
{
	physicssystem::ptr->set_gravity(gravity);
}