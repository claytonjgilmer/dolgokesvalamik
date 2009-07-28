#include "physicsshapenode.h"
#include "physics/system/body.h"
#include "physics/collision/shapes/shape.h"
#include "physics/system/physicssystem.h"
#include "render/rendersystem.h"

static vec3 light_dir(.5f,.5f,-.5f);

static void bind_light(mesh_t* mesh)
{
	for (unsigned int m=0; m<mesh->m_submeshbuf.size(); ++m)
	{
		mesh->m_submeshbuf[m].bind_param("light_dir",&light_dir,3*sizeof(f32));
	}
}


DEFINE_OBJECT(physics_shape_node,node_t);
BIND_PROPERTY(physics_shape_node,mass,"mass",float);
BIND_PROPERTY(physics_shape_node,friction,"friction",float);
BIND_PROPERTY(physics_shape_node,restitution,"restitution",float);

physics_shape_node::physics_shape_node()
{
	mass=1;
	friction=1;
	restitution=0;
	body=NULL;
}

shape_t* physics_shape_node::create_body(const shape_desc& sd)
{
	bodydesc bd;
	bd.mass=mass;
	bd.pos=get_worldposition();
	bd.inertia._11=inertia.x;
	bd.inertia._22=inertia.y;
	bd.inertia._33=inertia.z;
	
	body=physicssystem::ptr->create_body(bd);
	shape_t* shape=body->add_shape(sd);
	return shape;
}

void physics_shape_node::execute()
{
	if (body)
		set_worldposition(body->get_pos());
}

void physics_shape_node::exit()
{
	physicssystem::ptr->release_body(body);
}

DEFINE_OBJECT(box_shape_node,physics_shape_node);
BIND_PROPERTY(box_shape_node,extent,"extent",vec3);


box_shape_node::box_shape_node()
{
	extent.set(.5f,.5f,.5f);
}

mesh_t* generate_box(const char* texture_name, const vec3& extent, float uv_per_meter);
void box_shape_node::init()
{
	box_shape_desc bd;
	bd.extent=extent;
	bd.owner_flag=1;
	bd.collision_mask=1;
	
	if (mass>0)
	{
		float x=extent.y*extent.y+extent.z*extent.z;
		float y=extent.x*extent.x+extent.z*extent.z;
		float z=extent.x*extent.x+extent.y*extent.y;
		inertia.x=mass/3*x;
		inertia.y=mass/3*y;
		inertia.z=mass/3*z;
		shape=(box_shape*)create_body(bd);
	}
	else
	{
		bd.pos=get_worldposition();
		physicssystem::ptr->world->add_shape(bd);
	}

	mesh=generate_box("ground_02.dds",extent,1);
	bind_light(mesh);
}

void box_shape_node::render()
{
	mtx4x3 pos=get_worldposition();
//	pos.x*=2*extent.x;
//	pos.y*=2*extent.y;
//	pos.z*=2*extent.z;
	rendersystem::ptr->add_renderable(mesh,NULL,pos);
}

/*
DEFINE_OBJECT(sphere_shape_node,physics_shape_node);
BIND_PROPERTY(sphere_shape_node,radius,"radius",float);

*/