#ifndef _boxtower_h_
#define _boxtower_h_

#include "scene/node.h"

struct body_t;
struct mesh_t;
struct box_tower:node_t
{
	DECLARE_OBJECT(box_tower);
	box_tower();
	void init();
	void exit();
	void render();

	//props
	vec3 brick_extent;
	int tower_height;
	float tower_radius;

	float mass;
	float friction;
	float restitution;

	vector<body_t*> body_array;
	mesh_t* mesh;
};
#endif//_boxtower_h_