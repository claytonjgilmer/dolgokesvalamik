#ifndef _physicsshapenode_h_
#define _physicsshapenode_h_

#include "scene/node.h"

struct shape_t;
struct body_t;
struct shape_desc;
struct box_shape;
struct mesh_t;

struct physics_shape_node:node_t
{
	DECLARE_OBJECT(physics_shape_node);

	physics_shape_node();
	void execute();
	void exit();

	shape_t* create_body(const shape_desc& sd);

	body_t* body;
	vec3 inertia;

	//properties
	float mass;
	float friction;
	float restitution;
};

struct box_shape_node:physics_shape_node
{
	DECLARE_OBJECT(box_shape_node);
	box_shape_node();
	void init();
	void render();

	//props
	vec3 extent;
	box_shape* shape;
	mesh_t* mesh;
};
/*
struct sphere_shape_node:physics_shape_node
{
	DECLARE_OBJECT(sphere_shape_node);
	sphere_shape_node();
	void init();
	void render();

	//props
	float radius;
};
*/
#endif//_physicsshapenode_h_