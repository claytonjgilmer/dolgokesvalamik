#include "physicsshapenode.h"
#include "physics/system/body.h"
#include "physics/collision/shapes/shape.h"
#include "physics/system/physicssystem.h"
#include "render/rendersystem.h"
#include "math/geometry/convexhull.h"
#include "math/geometry/convexhullgeneration.h"
#include "render/renderobject3d.h"
#include "render/objectmanager.h"
#include "physics/collision/shapes/convexmeshshape.h"

static vec3 light_dir(.5f,.5f,-.5f);

static void bind_light(mesh_t* mesh)
{
	for (unsigned int m=0; m<mesh->m_submeshbuf.size(); ++m)
	{
		mesh->m_submeshbuf[m].bind_param("light_dir",&light_dir,3*sizeof(f32));
	}
}


DEFINE_OBJECT(physics_shape_node,node_t);
BIND_PROPERTY(physics_shape_node,mass,float);
BIND_PROPERTY(physics_shape_node,friction,float);
BIND_PROPERTY(physics_shape_node,restitution,float);

physics_shape_node::physics_shape_node()
{
	mass=1;
	friction=1;
	restitution=0;
	body=NULL;
}

shape_t* physics_shape_node::create_body(shape_desc& sd)
{
	bodydesc bd;
	bd.mass=mass;
	bd.pos=get_worldposition();
	bd.inertia._11=inertia.x;
	bd.inertia._22=inertia.y;
	bd.inertia._33=inertia.z;

	sd.owner_flag=1;
	sd.collision_mask=1;
	sd.friction=friction;
	sd.restitution=restitution;


	shape_t* shape;
	if (mass>0)
	{
		body=physicssystem::ptr->create_body(bd);
		shape=body->add_shape(sd);
	}
	else
	{
		shape=physicssystem::ptr->world->add_shape(sd);
	}
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
BIND_PROPERTY(box_shape_node,extent,vec3);


box_shape_node::box_shape_node()
{
	extent.set(.5f,.5f,.5f);
}

mesh_t* generate_box(const char* texture_name, const vec3& extent, float uv_per_meter);
void box_shape_node::init()
{
	box_shape_desc bd;
	bd.extent=extent;
	
	if (mass>0)
	{
		float x=extent.y*extent.y+extent.z*extent.z;
		float y=extent.x*extent.x+extent.z*extent.z;
		float z=extent.x*extent.x+extent.y*extent.y;
		inertia.x=mass/3*x;
		inertia.y=mass/3*y;
		inertia.z=mass/3*z;
	}
	else
	{
		bd.pos=get_worldposition();
	}

	shape=(box_shape*)create_body(bd);

	mesh=generate_box("Simaszurke.dds",extent,2);
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






DEFINE_OBJECT(convex_mesh_shape_node,physics_shape_node);
BIND_PROPERTY(convex_mesh_shape_node,object_name,string);

convex_mesh_shape_node::convex_mesh_shape_node()
{
	object_name="capsule.mmod";
	object=NULL;
}

void get_object_vertices(node_t* obj, const mtx4x3& mtx, vector<vec3>& vertbuf)
{
	if (obj->get_metaobject()->isa(object3d::get_class_metaobject()->get_typeid()))
	{
		object3d* obj3d=(object3d*)obj;

		for (unsigned n=0; n<obj3d->get_meshnum(); ++n)
		{
			mesh_t* m=obj3d->get_mesh(n);
			char* v=(char*)m->m_vb->lock();

			for (unsigned j=0; j<m->m_vb->m_vertexnum; ++j)
			{
				vertbuf.push_back(mtx.transform(*(vec3*)v));
				v+=m->m_vb->m_vertexsize;
			}

			m->m_vb->unlock();
		}
	}

	for (node_t* child=obj->get_child(); child; child=child->get_bro())
	{
		mtx4x3 cmtx; cmtx.multiply(child->get_localposition(),mtx);
		get_object_vertices(child,cmtx,vertbuf);
	}
}


void convex_mesh_shape_node::on_load()
{
	object=objectmanager::ptr->get_object(object_name.c_str());
	add_child(object);

	convex_hull_desc hd;
	hd.face_thickness=.00001;
	hd.vertex_min_dist=-.1;
	hd.triangle_output=false;
	hd.vertex_array.clear();
	get_object_vertices(object,mass > 0 ? mtx4x3::identitymtx() : this->local_pos,hd.vertex_array);
	convex_hull_generator ch;

	ch.do_all(hd);
	hull_to_convex_mesh(md,ch.ch);
}

void convex_mesh_shape_node::init()
{
	convex_mesh_shape_desc sd;
	sd.mesh_data=&md;

	if (mass>0)
	{
		vec3 extent=md.half_extent;
		float x=extent.y*extent.y+extent.z*extent.z;
		float y=extent.x*extent.x+extent.z*extent.z;
		float z=extent.x*extent.x+extent.y*extent.y;
		inertia.x=mass/3*x;
		inertia.y=mass/3*y;
		inertia.z=mass/3*z;
	}

	shape=(convex_mesh_shape_t*)create_body(sd);
//	object->set_worldposition(get_worldposition());
}

void convex_mesh_shape_node::render()
{
/*
	if (mass>0)
		object->set_worldposition(body->get_pos());

	object->render();
*/
}

/*
DEFINE_OBJECT(sphere_shape_node,physics_shape_node);
BIND_PROPERTY(sphere_shape_node,radius,"radius",float);

*/