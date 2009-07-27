#include "cameranode.h"

DEFINE_OBJECT(camera_node_t,node_t);
BIND_PROPERTY(camera_node_t,fov,"fov",angle);
BIND_PROPERTY(camera_node_t,near_z,"near_z",float);
BIND_PROPERTY(camera_node_t,far_z,"far_z",float);

camera_node_t::camera_node_t()
{
	fov=degreetorad(60);
	near_z=0.1f;
	far_z=1000.0f;
}

void camera_node_t::init()
{
	rendersystem::ptr->set_fov(fov);
	rendersystem::ptr->set_near_z(near_z);
	rendersystem::ptr->set_far_z(far_z);
	mtx4x3 invcammtx; invcammtx.linearinvert(this->get_worldposition());
	rendersystem::ptr->set_view_matrix((mtx4x4)invcammtx);
}

void camera_node_t::execute()
{
	mtx4x3 invcammtx; invcammtx.linearinvert(this->get_worldposition());
	rendersystem::ptr->set_view_matrix((mtx4x4)invcammtx);
}

//camera_node_t* g_cameranode(new camera_node_t);
//camera_node_t g_cameranode2;

