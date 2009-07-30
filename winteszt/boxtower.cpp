#include "boxtower.h"
#include "render/renderobject3d.h"
#include "physics/system/physicssystem.h"
#include "render/shadermanager.h"
#include "render/texturemanager.h"
#include "render/rendersystem.h"

DEFINE_OBJECT(box_tower,node_t);
BIND_PROPERTY(box_tower,brick_extent,"brick_extent",vec3);
BIND_PROPERTY(box_tower,tower_height,"tower_height",int);
BIND_PROPERTY(box_tower,tower_radius,"tower_radius",float);
BIND_PROPERTY(box_tower,friction,"friction",float);
BIND_PROPERTY(box_tower,restitution,"restitution",float);
BIND_PROPERTY(box_tower,mass,"mass",float);

box_tower::box_tower()
{
	brick_extent.set(1,1,1);
	tower_height=1;
	tower_radius=10;
	friction=1;
	restitution=0;
	mass=1;
}

mesh_t* generate_box(const char* texture_name, const vec3& extent, float uv_per_meter);
void box_tower::init()
{
	box_shape_desc sd;
	sd.extent=brick_extent;
	sd.friction=friction;
	sd.restitution=restitution;
	sd.owner_flag=1;
	sd.collision_mask=1;
	sd.pos.identity();
	float perimeter=tower_radius*2*pi();

	int brickNumPerLevel=(int)(perimeter/(2.5f*brick_extent.z));

	int brick_num=brickNumPerLevel*tower_height;

	body_array.resize(brick_num);

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

			bd.pos.multiply(brick_mtx,m_localpos);

			body_array[actBrick]=physicssystem::ptr->create_body(bd);
			body_array[actBrick]->add_shape(sd);
		}
	}

	mesh=generate_box("ground_02.dds",brick_extent,1);
}

void box_tower::exit()
{

}

void box_tower::render()
{
	for (unsigned n=0; n<body_array.size(); ++n)
		rendersystem::ptr->add_renderable(mesh,NULL,body_array[n]->get_pos());
}