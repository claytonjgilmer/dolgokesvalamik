#ifndef _meshtower_h_
#define _meshtower_h_

#include "scene/node.h"

struct body_t;
struct object3d;
struct mesh_tower:node_t
{
	DECLARE_OBJECT(mesh_tower);
	mesh_tower();
	void init();
	void exit();
	void render();

	float mass;
	float friction;
	float restitution;
	string object_name;

	vector<body_t*> body_array;
	object3d* object;

};
#endif//_meshtower_h_