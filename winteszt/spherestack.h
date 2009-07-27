#ifndef _spherestack_h_
#define _spherestack_h_

#include "scene/node.h"
#include "physics/system/body.h"


struct spherestack: node_t
{
	DECLARE_OBJECT(spherestack);
	spherestack();
	void init();
	void exit();
	void execute();
	void render();

	//properties
	int sphere_num;
	float sphere_radius;
	float sphere_dist;

	vector<body_t*> sphere_array;
	object3d* sphere_model;
};
#endif//_spherestack_h_