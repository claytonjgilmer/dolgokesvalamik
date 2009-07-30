#include "mlforsaken.h"
#include "game.h"
#include "input/inputsystem.h"

DEFINE_OBJECT(mlforsaken,node_t);

BIND_PROPERTY(mlforsaken,lin_vel,"lin_vel",float);
BIND_PROPERTY(mlforsaken,rot_vel,"rot_vel",float);

mlforsaken::mlforsaken()
{
	lin_vel=1;
	rot_vel=1;
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

	float ry=dt*inputsystem::ptr->GetMouseX()*rot_vel;
	float rx=dt*inputsystem::ptr->GetMouseY()*rot_vel;

	mtx4x3 lp=this->get_parent()->get_localposition(); lp.t+=dt*spx*lp.x; lp.t+=dt*spz*lp.z;
	float x,y,z;
	lp.get_euler(x,y,z);
	x+=rx;
	y+=ry;
	lp.set_euler(x,y,z);
	get_parent()->set_localposition(lp);
}