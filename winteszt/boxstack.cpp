#include "render/renderobject3d.h"
#include "physics/system/physicssystem.h"
#include "boxstack.h"

DEFINE_OBJECT(boxstack,node_t);
BIND_PROPERTY(boxstack,box_num,"box_num",int);
BIND_PROPERTY(boxstack,box_extent,"box_extent",vec3);
BIND_PROPERTY(boxstack,box_dist,"box_dist",float);

boxstack::boxstack()
{
	box_num=10;
	box_extent.set(1,1,1);
	box_dist=2;

	scale.set(1,1,1);
}

static vec3 light_dir(.5f,.5f,-.5f);

static void bind_light(node_t* node)
{
	if (node->get_metaobject()->isa(object3d::get_class_metaobject()->get_typeid()))
	{
		object3d* obj3d=(object3d*)node;

		for (unsigned n=0; n<obj3d->get_meshnum(); ++n)
		{
			mesh_t* mesh=obj3d->get_mesh(n);

			for (unsigned int m=0; m<mesh->m_submeshbuf.size(); ++m)
			{
				mesh->m_submeshbuf[m].bind_param("light_dir",&light_dir,3*sizeof(f32));
			}
		}
	}

	for (node_t* child=node->get_child(); child; child=child->get_bro())
	{
		bind_light(child);
	}

}
object3d* load_mmod(const char* i_filename);
void boxstack::init()
{/*
	box_shape_desc bsd;
	bsd.pos.identity();
	bsd.pos.t.set(0,-7,0);
	bsd.extent.set(1000,5,1000);
	bsd.owner_flag=1;
	bsd.collision_mask=1;
	bsd.restitution=1;
	bsd.friction=1;
	physicssystem::ptr->world->add_shape(bsd);
*/
	box_array.resize(box_num);

	const mtx4x3& mtx=this->get_worldposition();

	bodydesc bd;
	bd.mass=1;
	bd.inertia.identity();
	bd.pos=mtx;
	vec3 tr=mtx.t;


	for (int n=0; n<box_num;++n)
	{
		vec3 t(0,box_dist*(float)n,0);
		bd.pos.t=tr+t;

		box_array[n]=physicssystem::ptr->create_body(bd);
#ifdef _DEBUG
		char str[32];
		sprintf(str,"box%d",n+1);
		box_array[n]->set_name(str);
#endif

		box_shape_desc sd;
//		sd.center.clear();
		sd.pos.identity();
		sd.extent=box_extent;
		sd.owner_flag=1;
		sd.collision_mask=1;
		sd.restitution=0;
		sd.friction=1;
		box_array[n]->add_shape(sd);

	}

	box_model=load_mmod("model/box.mmod");
	bind_light(box_model);
	mesh_t* mesh=box_model->get_mesh(0);

	if (mesh)
	{
		scale=mesh->bounding.max-mesh->bounding.min;
	}

}

void boxstack::exit()
{
	for (int n=0; n<box_num; ++n)
		physicssystem::ptr->release_bodies(box_array.m_buf,box_array.size());

	box_array.resize(0);

}

void boxstack::execute()
{

}

void boxstack::render()
{
	for (int n=0; n<box_num;++n)
	{
		mtx4x3 pos=box_array[n]->get_pos();
		pos.x*=2*box_extent.x/scale.x;
		pos.y*=2*box_extent.y/scale.y;
		pos.z*=2*box_extent.z/scale.z;

		box_model->set_worldposition(pos);

		//		if (g_game->phb[n]->group_index==-1)
		//			box_model->color=color_f(1,1,1,1);
		//		else
		//			g_game->model[n]->color=color_buf[g_game->phb[n]->group_index % array_elemcount(color_buf)];
		box_model->render();
	}

}