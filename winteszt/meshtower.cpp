#include "meshtower.h"
#include "render/renderobject3d.h"
#include "physics/system/physicssystem.h"
#include "render/objectmanager.h"
#include "math/geometry/convexhull.h"
#include "math/geometry/convexhullgeneration.h"
#include "physics/collision/shapes/convexmeshshape.h"

DEFINE_OBJECT(mesh_tower,node_t);
BIND_PROPERTY(mesh_tower,mass,float);
BIND_PROPERTY(mesh_tower,friction,float);
BIND_PROPERTY(mesh_tower,restitution,float);
BIND_PROPERTY(mesh_tower,object_name,string);
BIND_PROPERTY(mesh_tower,tower_radius,float);
BIND_PROPERTY(mesh_tower,tower_height,int);

mesh_tower::mesh_tower()
{
	mass=1;
	friction=1;
	restitution=0;
	object_name="box.mmod";
}

void get_object_vertices(node_t* obj, const mtx4x3& mtx, vector<vec3>& vertbuf);

void mesh_tower::on_load()
{
	object=objectmanager::ptr->get_object(object_name.c_str());
//	add_child(object);

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


void mesh_tower::init()
{

	vec3 brick_extent=md.half_extent;
	float perimeter=tower_radius*2*pi();

	int brickNumPerLevel=(int)(perimeter/(2.5f*brick_extent.z));

	int brick_num=brickNumPerLevel*tower_height;

	body_array.resize(brick_num);

	convex_mesh_shape_desc sd;
	sd.mesh_data=&md;
	sd.owner_flag=sd.collision_mask=1;

	bodydesc bd;
	bd.mass=mass;

	bd.inertia.identity();
	bd.inertia.x.x=(mass/3.0f)*(brick_extent.y*brick_extent.y+brick_extent.z*brick_extent.z);
	bd.inertia.y.y=(mass/3.0f)*(brick_extent.x*brick_extent.x+brick_extent.z*brick_extent.z);
	bd.inertia.z.z=(mass/3.0f)*(brick_extent.y*brick_extent.y+brick_extent.x*brick_extent.x);

	mtx4x3 brick_mtx;
	brick_mtx.y.set(0,1,0);

	int actBrick=0;
	for (int n=0; n<tower_height;++n)
	{
		for (int m=0; m<brickNumPerLevel; ++m,++actBrick)
		{
			float angle=2*pi()*(m+0.5f*(n & 1))/brickNumPerLevel;
			brick_mtx.x.set(cos(angle),0,sin(angle));
			brick_mtx.t=tower_radius*brick_mtx.x;
			brick_mtx.t.y+=(2*n+1)*brick_extent.y;

			brick_mtx.z.cross(brick_mtx.x,brick_mtx.y);

			mtx4x3 absBrickMatrix;

			bd.pos.multiply(brick_mtx,local_pos);

			body_array[actBrick]=physicssystem::ptr->create_body(bd);
			body_array[actBrick]->add_shape(sd);
		}
	}
}

void mesh_tower::exit()
{

}

void mesh_tower::render()
{
	for (unsigned n=0; n<body_array.size(); ++n)
	{
		object->set_worldposition(body_array[n]->get_pos());
		object->render();
	}
}