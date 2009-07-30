#ifndef _boxstack_h_
#define _boxstack_h_

#include "scene/node.h"
#include "physics/system/body.h"

struct boxstack: node_t
{
	DECLARE_OBJECT(boxstack);
	boxstack();
	void init();
	void exit();
	void execute();
	void render();

	//properties
	int box_num;
	vec3 box_extent;
	float box_dist;
	float size_mul;

	vector<body_t*> box_array;
//	object3d* box_model;
	mesh_t* box_mesh;
};
#endif//_boxstack_h_