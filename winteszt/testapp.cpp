#include "stdafx.h"

#include <MMSystem.h>

#include "render/rendersystem.h"
#include "file/filesystem.h"
#include "threading/taskmanager.h"
#include "render/shadermanager.h"
#include "render/texturemanager.h"
#include "render/submesh.h"
#include "math/mtx4x4.h"
#include "math/vec2.h"
#include "render/renderobject3d.h"
#include "utils/auto_ptr.h"
#include "input/inputsystem.h"

void generate_sphere(math::vec3 o_pos[],int& o_numvertices,short o_indices[],int& o_numfaces,float i_radius, int i_depth);
render::object3d* load_mmod(const char* i_filename);
struct game
{
	render::mesh* m_mesh;
	render::mesh* sphere;
//	math::mtx4x3 m_mtx;

	float x,y,z;
	float camx,camy,camz;
	math::vec3 camt;
	float m_aspect;
	math::vec3 light_dir;
	render::object3d* obj;
	render::object3d* sky;

	game()
	{
		camz=camy=camx=0;
		camt.set(0,20,0);
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
HWND g_hwnd;

#define V math::vec3
#define V2 math::vec2

math::vec3 g_pos[]={V(-1,-1,-1),V(1,-1,-1),V(1,1,-1),V(-1,1,-1),V(-1,-1,1),V(1,-1,1),V(1,1,1),V(-1,1,1)};
math::vec2 g_uv[]={V2(0,0),V2(1,0),V2(1,1),V2(0,1),V2(1,0),V2(0,0),V2(0,1),V2(1,1)};

void init_app(HWND i_hwnd)
{
//	render::texture ttt(0,0,0);
	file::system::create();
	file::system::instance()->register_path("shader","c:\\data\\shader\\");
	file::system::instance()->register_path("texture","c:\\data\\texture\\");

	threading::taskmanagerdesc tdesc;
	threading::taskmanager::create(&tdesc);

	render::shadermanagerdesc shaderdesc("shader");
	render::shadermanager::create(&shaderdesc);

	input::inputinitparams params; params.m_Window=i_hwnd;
	input::system::create(&params);

	render::texturemanagerdesc textdesc("texture");
	render::texturemanager::create(&textdesc);

	render::systemdesc renderdesc;
	renderdesc.m_backbuffercount=2;
	RECT rect;
	GetClientRect(i_hwnd,&rect);
	renderdesc.m_screenwidth=abs(rect.right-rect.left);
	renderdesc.m_screenheight=abs(rect.bottom-rect.top);
	renderdesc.m_window=i_hwnd;
	renderdesc.m_windowed=true;

	render::system::create(&renderdesc);

	g_game.obj=	load_mmod("c:/data/model/test.mmod");
	g_game.sky= load_mmod("c:/data/model/skyBOX.MMOD");

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
	g_hwnd=i_hwnd;

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
	render::texture* txt=render::texturemanager::instance()->get_texture("white.bmp");
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

}







unsigned sumtime=0;

void update_app()
{
	threading::taskmanager::instance()->flush();
	unsigned acttime=::timeGetTime();
	unsigned deltatime=acttime-g_time;
	g_time=acttime;
	sumtime+=deltatime;
	float dt=(float)deltatime/1000.0f;

	if (sumtime>33)
	{
		input::system::instance()->Update();
		sumtime-=33;
	}


	if (sumtime>33 && 0)
	{
		sumtime-=33;
		unsigned width=abs(g_rect.left-g_rect.right);
		unsigned heigth=abs(g_rect.bottom-g_rect.top);
		::MoveWindow(g_hwnd,g_rect.left+(unsigned)(100*sin(acttime/1000.0)),g_rect.top,width,heigth,TRUE);
	}

	input::system* ip=input::system::instance();

	math::mtx4x3 cammtx=math::mtx4x3::identitymtx();
	cammtx.set_euler(g_game.camx,g_game.camy,g_game.camz);

	float speed;

	if (ip->KeyDown(KEYCODE_LSHIFT))
		speed=10;
	else
		speed=3;

	if (ip->KeyDown(KEYCODE_W))
		g_game.camt+=speed*dt*cammtx.zaxis;

	if (ip->KeyDown(KEYCODE_S))
		g_game.camt-=speed*dt*cammtx.zaxis;

	if (ip->KeyDown(KEYCODE_A))
		g_game.camt-=speed*dt*cammtx.xaxis;

	if (ip->KeyDown(KEYCODE_D))
		g_game.camt+=speed*dt*cammtx.xaxis;

	if (ip->KeyDown(KEYCODE_Q))
		g_game.camt+=speed*dt*cammtx.yaxis;

	if (ip->KeyDown(KEYCODE_Z))
		g_game.camt-=speed*dt*cammtx.yaxis;

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

	render::system::instance()->add_mesh(g_game.m_mesh,mtx);
	mtx.t.set(1,0,2.5f);
	render::system::instance()->add_mesh(g_game.sphere,mtx);
	mtx.set_euler(0,0,0);
	mtx.t.set(0,0,22.5f);
	g_game.obj->set_worldposition(mtx);
	g_game.obj->render();

//	math::mtx4x3 skymtx=g_game.sky->get_worldposition();
//	skymtx.normalize();
//	skymtx.axisx()*=0.01f;
//	skymtx.axisy()*=0.01f;
//	skymtx.axisz()*=0.01f;
//	g_game.sky->set_worldposition(skymtx);
	g_game.sky->render();
//	render::system::instance()->add_mesh(g_game.obj->get_mesh(),mtx);
	render::system::instance()->render();
}

void exit_app()
{
	file::system::release();
	threading::taskmanager::release();
	delete g_game.m_mesh;
	delete g_game.sphere;
	delete g_game.obj;
	delete g_game.sky;
	render::shadermanager::release();
	render::texturemanager::release();
	input::system::release();
	render::system::release();
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
	if (render::shadermanager::instance())
		render::shadermanager::instance()->reload_shaders();
}