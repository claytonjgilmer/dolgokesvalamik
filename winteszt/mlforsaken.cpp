#include "mlforsaken.h"
#include "game.h"
#include "input/inputsystem.h"
#include "math/inertia.h"

DEFINE_OBJECT(mlforsaken,node_t);

BIND_PROPERTY(mlforsaken,lin_vel,float);
BIND_PROPERTY(mlforsaken,rot_vel,float);

mlforsaken::mlforsaken()
{
	lin_vel=1;
	rot_vel=1;

	body=NULL;
	sphere_radius=1;
	mass=1;
}

void mlforsaken::init()
{
	sphere_shape_desc sd;
	sd.radius=sphere_radius;
	sd.center.clear();
	sd.friction=0;
	sd.restitution=0;
	sd.owner_flag=sd.collision_mask=1;

	bodydesc bd;
	bd.mass=mass;
	bd.inertia=get_inertia_sphere(mass,sphere_radius);
	bd.pos=get_parent()->get_worldposition();

	body=physicssystem::ptr->create_body(bd);
	body->add_shape(sd);

}

void mlforsaken::execute()
{
	float dt=g_game->frame_time;
	float spx=0,spz=0;
	if (inputsystem::ptr->KeyDown(KEYCODE_W))
		spz=lin_vel;
	if (inputsystem::ptr->KeyDown(KEYCODE_S))
		spz=-lin_vel;
	if (inputsystem::ptr->KeyDown(KEYCODE_A))
		spx=-lin_vel;
	if (inputsystem::ptr->KeyDown(KEYCODE_D))
		spx=lin_vel;

	if (inputsystem::ptr->MouseButtonDown(0))
	{
		spx*=5;
		spz*=5;
	}

	float ry=inputsystem::ptr->GetMouseX()*rot_vel;
	float rx=inputsystem::ptr->GetMouseY()*rot_vel;

/*

	mtx4x3 lp=this->get_parent()->get_localposition(); lp.t+=dt*spx*lp.x; lp.t+=dt*spz*lp.z;
	float x,y,z;
	lp.get_euler(x,y,z);
	x+=rx;
	y+=ry;
	lp.set_euler(x,y,z);
*/

	get_parent()->set_localposition(body->get_pos());
	if (spx!=0 || spz!=0)
		strlen("");
	const mtx4x3& lp=body->get_pos();
	body->set_vel(spx*lp.x+spz*lp.z);
	body->set_rotvel(vec3(0,ry,0)+rx*lp.x);
}