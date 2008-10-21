#include "stdafx.h"

#include <MMSystem.h>

#include "render/rendersystem.h"
#include "file/filesystem.h"
#include "threading/taskmanager2.h"
#include "render/shadermanager.h"
#include "render/texturemanager.h"
#include "render/submesh.h"
#include "math/mtx4x4.h"
#include "math/vec2.h"
#include "render/renderobject3d.h"
#include "utils/auto_ptr.h"
#include "input/inputsystem.h"
#include "physics/system/physicssystem.h"

void generate_sphere(math::vec3 o_pos[],int& o_numvertices,short o_indices[],int& o_numfaces,float i_radius, int i_depth);
render::object3d* load_mmod(const char* i_filename);
struct game
{
	utils::ref_ptr<render::mesh> m_mesh;
	utils::ref_ptr<render::mesh> sphere;
//	math::mtx4x3 m_mtx;

	float x,y,z;
	float camx,camy,camz;
	math::vec3 camt;
	float m_aspect;
	math::vec3 light_dir;
	render::object3d* obj;
	render::object3d* sky;
#define BODY_NUM 500
#define ROOM_SIZE 10.0f
	physics::body_t* phb[BODY_NUM];
	bool inited;

	game()
	{
		camz=camy=camx=0;
		camt.set(0,0,-10);
		inited=false;
	}
} g_game;

void change_screen_size(unsigned i_newwidth, unsigned i_newheight)
{
	if (!i_newheight || !i_newwidth)
		return;
	g_game.m_aspect=i_newwidth/(float)i_newheight;
}


RECT g_rect;
unsigned g_time;
//HWND g_hwnd;

#define V math::vec3
#define V2 math::vec2

math::vec3 g_pos[]={V(-1,-1,-1),V(1,-1,-1),V(1,1,-1),V(-1,1,-1),V(-1,-1,1),V(1,-1,1),V(1,1,1),V(-1,1,1)};
math::vec2 g_uv[]={V2(0,0),V2(1,0),V2(1,1),V2(0,1),V2(1,1),V2(0,1),V2(0,0),V2(1,0)};

void init_app(HWND i_hwnd)
{
	file::system::create();
	file::system::instance()->register_path("shader","shader\\");
	file::system::instance()->register_path("texture","texture\\");

	threading::taskmanagerdesc tdesc;
	tdesc.m_threadnum=3;
	threading::taskmanager::create(&tdesc);

	render::shadermanagerdesc shaderdesc("shader");
	render::shadermanager::create(&shaderdesc);

	input::inputinitparams params; params.m_Window=i_hwnd;
	input::system::create(&params);

	render::texturemanagerdesc textdesc("texture");
	render::texturemanager::create(&textdesc);

	physics::systemdesc pd;
	physics::system::create(&pd);

	render::systemdesc renderdesc;
	renderdesc.m_backbuffercount=2;
	RECT rect;
	GetClientRect(i_hwnd,&rect);
	renderdesc.m_screenwidth=abs(rect.right-rect.left);
	renderdesc.m_screenheight=abs(rect.bottom-rect.top);
	renderdesc.m_window=i_hwnd;
	renderdesc.m_windowed=true;

	render::system::create(&renderdesc);

	g_game.obj=	load_mmod("model/box.mmod");
	g_game.sky= load_mmod("model/skyBOX.MMOD");

	for (unsigned n=0; n<g_game.sky->get_meshnum();++n)
	{
		render::mesh* m=g_game.sky->get_mesh(n);

		for (unsigned k=0; k<m->m_submeshbuf.size(); ++k)
		{
			render::submesh& sm=m->m_submeshbuf[k];

			sm.set_shader(render::shadermanager::instance()->get_shader("nothing.fx"));
		}
	}


	g_game.m_aspect=(float)renderdesc.m_screenwidth/(float)renderdesc.m_screenheight;

	::GetWindowRect(i_hwnd,&g_rect);
	g_time=timeGetTime();
//	g_hwnd=i_hwnd;

//	math::mtx4x4 mtx; mtx.set_projectionmatrix(tan(math::degreetorad(45)),g_game.m_aspect,1,10000);

	render::mesh* mesh=new render::mesh("fucka");

	ctr::vector<render::vertexelem> ve;

	ve.push_back(render::vertexelem(render::vertexelement_position,3));
	ve.push_back(render::vertexelem(render::vertexelement_normal,3));
	ve.push_back(render::vertexelem(render::vertexelement_uv,2));

	mesh->m_vb=new render::vertexbuffer(8,ve);
	mesh->m_ib=new render::indexbuffer(3*12);

	class vertex_t
	{
	public:
		math::vec3 pos;
		math::vec3 normal;
		math::vec2 uv;
	};

	vertex_t* vb=(vertex_t*)mesh->m_vb->lock();

	for (int n=0; n<8; ++n)
	{
		vb[n].pos=0.1f*g_pos[n];
		vb[n].uv=g_uv[n];
		vb[n].normal=g_pos[n];  vb[n].normal.normalize();
	}


	mesh->m_vb->unlock();

	unsigned short* ib=(unsigned short*)mesh->m_ib->lock();

	const unsigned short buf[]={0,1,2,0,2,3,1,5,6,1,6,2,2,6,7,2,7,3,4,0,3,4,3,7,5,4,7,5,7,6,1,0,4,1,4,5};

	memcpy(ib,buf,36*sizeof(short));

	mesh->m_ib->unlock();


	mesh->m_submeshbuf.resize(1);
	render::submesh& ts=mesh->m_submeshbuf.back();
	ts.m_firstindex=0;
	ts.m_numindices=36;
	ts.m_firstvertex=0;
	ts.m_numvertices=8;

	ts.set_shader(render::shadermanager::instance()->get_shader("posnormuv.fx"));
	ts.bind_param("light_dir",&g_game.light_dir,3*sizeof(float));


//	render::texture* txt=render::texturemanager::instance()->get_texture("teszt.jpg");
	render::texture* txt=render::texturemanager::instance()->get_texture("structures_malayhouse.dds");
	ts.m_texturebuf.push_back(txt);

	g_game.x=g_game.y=g_game.z=0;

	g_game.m_mesh=mesh;


	math::vec3 pos[15000];
	int posnum;
	short index[16384];
	int indexnum;

	generate_sphere(pos,posnum,index,indexnum,0.1f,5);
	g_game.sphere=new render::mesh("sphere");
	g_game.sphere->m_vb=new render::vertexbuffer(posnum,ve);
	g_game.sphere->m_ib=new render::indexbuffer(indexnum*3);
	vb=(vertex_t*)g_game.sphere->m_vb->lock();

	for (int n=0; n<posnum; ++n)
	{
		vb[n].pos=pos[n];
		vb[n].normal=pos[n];
		vb[n].normal.normalize();
		vb[n].uv.set(0,0);
	}
	g_game.sphere->m_vb->unlock();

	ib=(unsigned short*)g_game.sphere->m_ib->lock();
	memcpy(ib,index,indexnum*3*sizeof(short));
	g_game.sphere->m_ib->unlock();

	g_game.sphere->m_submeshbuf.resize(1);
	render::submesh& sts=g_game.sphere->m_submeshbuf.back();
	sts.m_firstindex=0;
	sts.m_numindices=indexnum*3;
	sts.m_firstvertex=0;
	sts.m_numvertices=posnum;

	sts.set_shader(render::shadermanager::instance()->get_shader("posnormuv.fx"));
	sts.bind_param("light_dir",&g_game.light_dir,3*sizeof(float));


	//	render::texture* txt=render::texturemanager::instance()->get_texture("teszt.jpg");
//	render::texture* txt=render::texturemanager::instance()->get_texture("white.bmp");
	sts.m_texturebuf.push_back(txt);


	physics::bodydesc bd;
	bd.mass=1;
	bd.inertia.identity();
	bd.is_static=physics::BODYSTATE_DYNAMIC;


	for (unsigned n=0; n<BODY_NUM;++n)
	{
		float x=math::random(-ROOM_SIZE,ROOM_SIZE);
		float y=math::random(-ROOM_SIZE,ROOM_SIZE);
		float z=math::random(-ROOM_SIZE,ROOM_SIZE);
		bd.pos.t.set(x,y,z);
		x=math::random(-3.0f,3.0f);
		y=math::random(-3.0f,3.0f);
		z=math::random(-3.0f,3.0f);
		bd.vel.set(x,y,z);
		bd.rotvel.set(x/3,y/3,z/3);

		g_game.phb[n]=physics::system::instance()->create_body(bd);
	}


	g_game.inited=true;
}







unsigned sumtime=0;

void update_app()
{
	threading::taskmanager::instance()->flush();
	unsigned acttime=::timeGetTime();
	unsigned deltatime=min(acttime-g_time,100);
	g_time=acttime;
	sumtime+=deltatime;
	float dt=(float)deltatime/1000.0f;

	if (sumtime>33)
	{
		input::system::instance()->Update();
		sumtime-=33;
	}

	physics::system::instance()->simulate(dt);

	input::system* ip=input::system::instance();

	math::mtx4x3 cammtx=math::mtx4x3::identitymtx();
	cammtx.set_euler(g_game.camx,g_game.camy,g_game.camz);

	float speed;

	if (ip->KeyDown(KEYCODE_LSHIFT))
		speed=10;
	else
		speed=3;

	if (ip->KeyDown(KEYCODE_W))
		g_game.camt+=speed*dt*cammtx.z;

	if (ip->KeyDown(KEYCODE_S))
		g_game.camt-=speed*dt*cammtx.z;

	if (ip->KeyDown(KEYCODE_A))
		g_game.camt-=speed*dt*cammtx.x;

	if (ip->KeyDown(KEYCODE_D))
		g_game.camt+=speed*dt*cammtx.x;

	if (ip->KeyDown(KEYCODE_Q))
		g_game.camt+=speed*dt*cammtx.y;

	if (ip->KeyDown(KEYCODE_Z))
		g_game.camt-=speed*dt*cammtx.y;

	int mx=ip->GetMouseX();
	int my=ip->GetMouseY();


	g_game.camy+=dt*(float)ip->GetMouseX()/10.0f;
	g_game.camx+=dt*(float)ip->GetMouseY()/10.0f;

	cammtx.t=g_game.camt;

	math::mtx4x3 viewmtx; viewmtx.linearinvert(cammtx);

	render::system::instance()->set_projection_params(math::degreetorad(60),g_game.m_aspect,0.3f,10000,(math::mtx4x4)viewmtx);

	math::mtx4x3 mtx;
	mtx.set_euler(g_game.x,g_game.y,g_game.z);
	mtx.t.set(-1,0,2.5f);

	g_game.obj->get_worldposition().transformtransposed3x3(g_game.light_dir,math::vec3(1,1,0));
	g_game.light_dir.normalize();

	g_game.x+=dt/10;
	g_game.y+=2*dt/10;
	g_game.z+=3*dt/10;

	for (unsigned n=0; n<BODY_NUM;++n)
	{
		math::mtx4x3 pos=g_game.phb[n]->get_pos();
		math::vec3 vel=g_game.phb[n]->get_vel();

		if (pos.t.x<-ROOM_SIZE)
		{
			pos.t.x=-ROOM_SIZE;
			vel.x=math::abs(vel.x);
		}
		if (pos.t.y<-ROOM_SIZE)
		{
			pos.t.y=-ROOM_SIZE;
			vel.y=math::abs(vel.y);
		}
		if (pos.t.z<-ROOM_SIZE)
		{
			pos.t.z=-ROOM_SIZE;
			vel.z=math::abs(vel.z);
		}

		if (pos.t.x>ROOM_SIZE)
		{
			pos.t.x=ROOM_SIZE;
			vel.x=-math::abs(vel.x);
		}
		if (pos.t.y>ROOM_SIZE)
		{
			pos.t.y=ROOM_SIZE;
			vel.y=-math::abs(vel.y);
		}
		if (pos.t.z>ROOM_SIZE)
		{
			pos.t.z=ROOM_SIZE;
			vel.z=-math::abs(vel.z);
		}

		g_game.phb[n]->set_pos(pos);
		g_game.phb[n]->set_vel(vel);

		render::system::instance()->add_mesh(g_game.m_mesh.get(),pos);
	}

	render::system::instance()->add_mesh(g_game.m_mesh.get(),mtx);
	mtx.t.set(1,0,2.5f);
	render::system::instance()->add_mesh(g_game.sphere.get(),mtx);
	mtx.set_euler(0,0,0);
	mtx.t.set(0,0,22.5f);
	g_game.obj->set_worldposition(mtx);
	g_game.obj->render();

	g_game.sky->render();
	render::system::instance()->render();
}

void exit_app()
{
	file::system::release();
	threading::taskmanager::release();
	g_game.m_mesh=NULL;
	g_game.sphere=NULL;
	delete g_game.obj;
	delete g_game.sky;
	render::shadermanager::release();
	render::texturemanager::release();
	input::system::release();
	render::system::release();
	physics::system::release();
}

void generate_tetrahedron(math::vec3 o_pos[],float i_radius)
{
	float Pi = 3.141592653589793238462643383279502884197f;

	float phiaa  = -19.471220333f; /* the phi angle needed for generation */

	float phia = Pi*phiaa/180.0f; /* 1 set of three points */
	float the120 = Pi*120.0f/180.0f;
	o_pos[0][0] = 0.0f;
	o_pos[0][1] = 0.0f;
	o_pos[0][2] = i_radius;
	float the = 0.0f;
	for(int i=1; i<4; i++)
	{
		o_pos[i][0]=i_radius*cos(the)*cos(phia);
		o_pos[i][1]=i_radius*sin(the)*cos(phia);
		o_pos[i][2]=i_radius*sin(phia);
		the = the+the120;
	}
}

void generate_sphere(math::vec3 o_pos[],int& o_numvertices,short o_indices[],int& o_numfaces,float i_radius, int i_depth)
{
	generate_tetrahedron(o_pos,i_radius);

	o_numvertices=4;
	o_numfaces=4;
	o_indices[0]=0;o_indices[1]=1;o_indices[2]=2;
	o_indices[3]=0;o_indices[4]=2;o_indices[5]=3;
	o_indices[6]=0;o_indices[7]=3;o_indices[8]=1;
	o_indices[9]=1;o_indices[10]=2;o_indices[11]=3;

	while (i_depth)
	{
		int actnumfaces=o_numfaces;
		for (int n=0; n<actnumfaces; ++n)
		{
			int i1=o_indices[3*n];
			int i2=o_indices[3*n+1];
			int i3=o_indices[3*n+2];

			o_pos[o_numvertices]=(o_pos[i1]+o_pos[i2]+o_pos[i3])/3;
			o_pos[o_numvertices].normalize();
			o_pos[o_numvertices]*=i_radius;

			int uj=o_numvertices;
			o_indices[3*n+2]=uj;

			o_indices[3*o_numfaces]=uj;
			o_indices[3*o_numfaces+1]=i2;
			o_indices[3*o_numfaces+2]=i3;
			++o_numfaces;

			o_indices[3*o_numfaces]=i1;
			o_indices[3*o_numfaces+1]=uj;
			o_indices[3*o_numfaces+2]=i3;
			++o_numfaces;

			++o_numvertices;
		}
		--i_depth;
	}
}

void reload_shaders()
{
	if (g_game.inited)
		render::shadermanager::instance()->reload_shaders();
}