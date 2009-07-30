#include "render/renderobject3d.h"
#include "physics/system/physicssystem.h"
#include "boxstack.h"
#include "render/shadermanager.h"
#include "render/texturemanager.h"
#include "render/rendersystem.h"

DEFINE_OBJECT(boxstack,node_t);
BIND_PROPERTY(boxstack,box_num,"box_num",int);
BIND_PROPERTY(boxstack,box_extent,"box_extent",vec3);
BIND_PROPERTY(boxstack,box_dist,"box_dist",float);
BIND_PROPERTY(boxstack,size_mul,"size_mul",float);

mesh_t* generate_box(const char* texture_name, const vec3& extent, float uv_per_meter)
{
	mesh_t* mesh=new mesh_t("box");
	int trinum=12;
	int vertexnum=24;
	indexbuffer* IB=new indexbuffer(36,0);
	mesh->set_indexbuffer(IB);

	
	short* index=(short*)IB->lock();

	*index++=0;
	*index++=2;
	*index++=1;

	*index++=0;
	*index++=3;
	*index++=2;

	*index++=4;
	*index++=6;
	*index++=5;

	*index++=4;
	*index++=7;
	*index++=6;

	*index++=8;
	*index++=10;
	*index++=9;

	*index++=8;
	*index++=11;
	*index++=10;

	*index++=12;
	*index++=14;
	*index++=13;

	*index++=12;
	*index++=15;
	*index++=14;

	*index++=16;
	*index++=18;
	*index++=17;

	*index++=16;
	*index++=19;
	*index++=18;

	*index++=20;
	*index++=22;
	*index++=21;

	*index++=20;
	*index++=23;
	*index++=22;

	IB->unlock();

	vector<vertexelem> vdecl;
	vdecl.push_back(vertexelem(vertexelement_position,3));
	vdecl.push_back(vertexelem(vertexelement_normal,3));
	vdecl.push_back(vertexelem(vertexelement_uv,2));

	vertexbuffer* VB=new vertexbuffer(vertexnum,vdecl);
	mesh->set_vertexbuffer(VB);

	int poffs=0;
	int normoffs=12;
	int uvoffs=24;
	int vsize=32;

	char* ptr=(char*)VB->lock();
	char* vptr=ptr+poffs;
	char* uvptr=ptr+uvoffs;
	char* normptr=ptr+normoffs;

	static float nnn[]=
	{
		0,0,-1,
		0,0,-1,
		0,0,-1,
		0,0,-1,

		0,0,1,
		0,0,1,
		0,0,1,
		0,0,1,

		1,0,0,
		1,0,0,
		1,0,0,
		1,0,0,

		-1,0,0,
		-1,0,0,
		-1,0,0,
		-1,0,0,

		0,1,0,
		0,1,0,
		0,1,0,
		0,1,0,

		0,-1,0,
		0,-1,0,
		0,-1,0,
		0,-1,0,
	};

	static float ppp[]=
	{
		//elol
		-1,-1,-1,
		1,-1,-1,
		1,1,-1,
		-1,1,-1,

		//hatul
		-1,-1,1,
		-1,1,1,
		1,1,1,
		1,-1,1,

		//jobb
		1,-1,-1,
		1,-1,1,
		1,1,1,
		1,1,-1,

		//bal
		-1,-1,1,
		-1,-1,-1,
		-1,1,-1,
		-1,1,1,

		//fent
		-1,1,-1,
		1,1,-1,
		1,1,1,
		-1,1,1,

		//lent
		-1,-1,1,
		1,-1,1,
		1,-1,-1,
		-1,-1,-1,

	};

	vec3 uvMul = (uv_per_meter== 0 ? vec3(1,1,1) : extent/uv_per_meter);

	float uv[]=
	{
		0,0,
		uvMul.x,0,
		uvMul.x,uvMul.y,
		0,uvMul.y,

		0,0,
		uvMul.x,0,
		uvMul.x,uvMul.y,
		0,uvMul.y,

		0,0,
		uvMul.z,0,
		uvMul.z,uvMul.y,
		0,uvMul.y,

		0,0,
		uvMul.z,0,
		uvMul.z,uvMul.y,
		0,uvMul.y,

		0,0,
		uvMul.x,0,
		uvMul.x,uvMul.z,
		0,uvMul.z,

		0,0,
		uvMul.x,0,
		uvMul.x,uvMul.z,
		0,uvMul.z,
	};

	for (int n=0; n<vertexnum; ++n,vptr+=vsize,normptr+=vsize,uvptr+=vsize)
	{
		vec3& vec=*(vec3*)vptr;
		float* UV=(float*)uvptr;
		vec3& norm=*(vec3*)normptr;

		vec.x=ppp[3*n]*extent.x;
		vec.y=ppp[3*n+1]*extent.y;
		vec.z=ppp[3*n+2]*extent.z;

		UV[0]=uv[2*n];
		UV[1]=uv[2*n+1];

		norm.x=nnn[3*n];
		norm.y=nnn[3*n+1];
		norm.z=nnn[3*n+2];
	}

	VB->unlock();

	mesh->m_submeshbuf.push_back(submesh());
	submesh& sm=mesh->m_submeshbuf.back();
	sm.set_shader(shadermanager::ptr->get_shader("posnormuv.fx"));
	sm.m_firstindex=0;
	sm.m_numindices=36;
	sm.m_firstvertex=0;
	sm.m_numvertices=24;
	sm.m_texturebuf.push_back(texturemanager::ptr->get_texture(texture_name));

	return mesh;
}


boxstack::boxstack()
{
	box_num=10;
	box_extent.set(1,1,1);
	box_dist=2;
	size_mul=1;
}

static vec3 light_dir(.5f,.5f,-.5f);

static void bind_light(mesh_t* mesh)
{
	for (unsigned int m=0; m<mesh->m_submeshbuf.size(); ++m)
	{
		mesh->m_submeshbuf[m].bind_param("light_dir",&light_dir,3*sizeof(f32));
	}
}

void boxstack::init()
{
	box_array.resize(box_num);

	const mtx4x3& mtx=this->get_worldposition();

	bodydesc bd;
	bd.mass=1;
	bd.inertia.identity();
	bd.pos=mtx;
	vec3 tr=mtx.t;

	vec3 extent=box_extent;


	for (int n=0; n<box_num;++n)
	{
		vec3 t(box_dist*(float)n*mtx.y);
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
		sd.extent=extent;
		extent.x*=size_mul;
		extent.z*=size_mul;
		sd.owner_flag=1;
		sd.collision_mask=1;
		sd.restitution=0;
		sd.friction=1;
		box_array[n]->add_shape(sd);

	}

	box_mesh=generate_box("ground_02.dds",vec3(1,1,1),1);
	bind_light(box_mesh);
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
	vec3 extent=box_extent;
	for (int n=0; n<box_num;++n)
	{
		mtx4x3 m=box_array[n]->get_pos();
		m.x*=extent.x;
		m.y*=extent.y;
		m.z*=extent.z;
		extent.x*=size_mul;
		extent.z*=size_mul;
		rendersystem::ptr->add_renderable(box_mesh,NULL,m);
	}
}