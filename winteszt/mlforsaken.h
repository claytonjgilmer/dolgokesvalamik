#ifndef _mlforsaken_h_
#define _mlforsaken_h_

#include "scene/node.h"

struct body_t;
struct mlforsaken:node_t
{
	DECLARE_OBJECT(mlforsaken);

	mlforsaken();
	void init();
	void execute();

	float lin_vel;
	float rot_vel;



	float mass;
	float sphere_radius;

	body_t* body;
};
#endif//_mlforsaken_h_