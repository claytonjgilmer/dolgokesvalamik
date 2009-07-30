#ifndef _physicsshapenode_h_
#define _physicsshapenode_h_

#include "scene/node.h"
#include "physics/collision/shapes/convexmeshdata.h"


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

	shape_t* create_body(shape_desc& sd);

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



struct convex_mesh_shape_t;
struct object3d;
struct convex_mesh_shape_node:physics_shape_node
{
	DECLARE_OBJECT(convex_mesh_shape_node);
	convex_mesh_shape_node();
	void on_load();
	void init();
	void render();

	string object_name;


	convex_mesh_shape_t* shape;
	object3d* object;

	convex_mesh_data_t md;
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