#ifndef _cameranode_h_
#define _cameranode_h_

#include "scene/node.h"

struct camera_node_t:node_t
{
	DECLARE_OBJECT(camera_node_t);
	camera_node_t();

	void init();
	void execute();

	//properties
	float fov;
	float near_z;
	float far_z;
};
#endif//_cameranode_h_