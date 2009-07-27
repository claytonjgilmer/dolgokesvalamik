#include "render/renderobject3d.h"
#include "physics/system/physicssystem.h"
#include "spherestack.h"

DEFINE_OBJECT(spherestack,node_t);
BIND_PROPERTY(spherestack,sphere_num,"sphere_num",int);
BIND_PROPERTY(spherestack,sphere_radius,"sphere_radius",float);
BIND_PROPERTY(spherestack,sphere_dist,"sphere_dist",float);

spherestack::spherestack()
{
	sphere_num=10;
	sphere_radius=1;
	sphere_dist=2;
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
void spherestack::init()
{
	box_shape_desc bsd;
	bsd.pos.identity();
	bsd.pos.t.set(0,-7,0);
	bsd.extent.set(1000,5,1000);
	bsd.owner_flag=1;
	bsd.collision_mask=1;
	bsd.restitution=1;
	bsd.friction=1;
	physicssystem::ptr->world->add_shape(bsd);

	sphere_array.resize(sphere_num);

	const mtx4x3& mtx=this->get_worldposition();

	bodydesc bd;
	bd.mass=1;
	bd.inertia.identity();
	bd.pos=mtx;
	vec3 tr=mtx.t;


	for (int n=0; n<sphere_num;++n)
	{
		vec3 t(0,sphere_dist*(float)n,0);
		bd.pos.t=tr+t;

		sphere_array[n]=physicssystem::ptr->create_body(bd);
#ifdef _DEBUG
		char str[32];
		sprintf(str,"SPHERE%d",n+1);
		sphere_array[n]->set_name(str);
#endif

		sphere_shape_desc sd;
		sd.center.clear();
		sd.radius=sphere_radius;
		sd.owner_flag=1;
		sd.collision_mask=1;
		sd.restitution=0;
		sd.friction=1;
		sphere_array[n]->add_shape(sd);

	}

	sphere_model=load_mmod("model/sphere.mmod");
	bind_light(sphere_model);

}

void spherestack::exit()
{
	for (int n=0; n<sphere_num; ++n)
		physicssystem::ptr->release_bodies(sphere_array.m_buf,sphere_array.size());

	sphere_array.resize(0);
	
}

void spherestack::execute()
{

}

void spherestack::render()
{
	for (int n=0; n<sphere_num;++n)
	{
		mtx4x3 pos=sphere_array[n]->get_pos();
		pos.x*=sphere_radius;
		pos.y*=sphere_radius;
		pos.z*=sphere_radius;

		sphere_model->set_worldposition(pos);

//		if (g_game->phb[n]->group_index==-1)
//			sphere_model->color=color_f(1,1,1,1);
//		else
//			g_game->model[n]->color=color_buf[g_game->phb[n]->group_index % array_elemcount(color_buf)];
		sphere_model->render();
	}

}