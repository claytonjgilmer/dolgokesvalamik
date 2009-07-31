#include "meshtower.h"
#include "render/renderobject3d.h"
#include "physics/system/physicssystem.h"

DEFINE_OBJECT(mesh_tower,node_t);
BIND_PROPERTY(mesh_tower,mass,float);
BIND_PROPERTY(mesh_tower,friction,float);
BIND_PROPERTY(mesh_tower,restitution,float);
BIND_PROPERTY(mesh_tower,object_name,string);

mesh_tower::mesh_tower()
{
	mass=1;
	friction=1;
	restitution=0;
	object_name="box.mmod";
}

void mesh_tower::init()
{

}

void mesh_tower::exit()
{

}

void mesh_tower::render()
{

}