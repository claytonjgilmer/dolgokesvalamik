#ifndef _meshtower_h_
#define _meshtower_h_

#include "scene/node.h"
#include "physics/collision/shapes/convexmeshdata.h"

struct body_t;
struct object3d;
struct mesh_tower:node_t
{
	DECLARE_OBJECT(mesh_tower);
	mesh_tower();
	void on_load();
	void init();
	void exit();
	void render();

	//props
	float mass;
	float friction;
	float restitution;
	string object_name;
	int tower_height;
	float tower_radius;

	vector<body_t*> body_array;
	object3d* object;

	convex_mesh_data_t md;

};
#endif//_meshtower_h_