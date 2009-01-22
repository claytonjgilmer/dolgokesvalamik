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
#include "utils/timer.h"
#include "input/inputsystem.h"
#include "physics/system/physicssystem.h"
#include "math/convexhull.h"
#include "math/convexhullgeneration.h"
#include "utils/timer.h"

void generate_sphere(vec3 o_pos[],int& o_numvertices,short o_indices[],int& o_numfaces,float i_radius, int i_depth);
object3d* load_mmod(const char* i_filename);
struct game
{
	ref_ptr<mesh> m_mesh;
	ref_ptr<mesh> sphere;
//	mtx4x3 m_mtx;

	float x,y,z;
	float camx,camy,camz;
	vec3 camt;
	float m_aspect;
	vec3 light_dir;
	object3d* obj;
	object3d* sky;
#define BODY_NUM 50
#define ROOM_SIZE 10.0f
#define BODY_SIZE .5f
	body_t* phb[BODY_NUM];
	int inited;
	timer t;

	convex_hull ch;

	game()
	{
		camz=camy=camx=0;
		camt.set(0,0,-10);
		inited=false;
		t.reset();
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

#define V vec3
#define V2 vec2

vec3 g_pos[8]={{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};
vec2 g_uv[]={V2(0,0),V2(1,0),V2(1,1),V2(0,1),V2(1,1),V2(0,1),V2(0,0),V2(1,0)};


void init_app(HWND i_hwnd)
{
	filesystem::create();
	filesystem::ptr->register_path("shader","shader\\");
	filesystem::ptr->register_path("texture","texture\\");

	taskmanagerdesc tdesc;
	tdesc.m_threadnum=50;
	taskmanager::create(&tdesc);

	shadermanagerdesc shaderdesc("shader");
	shadermanager::create(&shaderdesc);

	inputinitparams params; params.m_Window=i_hwnd;
	inputsystem::create(&params);

	texturemanagerdesc textdesc("texture");
	texturemanager::create(&textdesc);

	physicssystemdesc pd;
	pd.gravity.set(0,0,0);
	physicssystem::create(&pd);

	rendersystemdesc renderdesc;
	renderdesc.m_backbuffercount=2;
	RECT rect;
	GetClientRect(i_hwnd,&rect);
	renderdesc.m_screenwidth=abs(rect.right-rect.left);
	renderdesc.m_screenheight=abs(rect.bottom-rect.top);
	renderdesc.m_window=i_hwnd;
	renderdesc.m_windowed=true;

	rendersystem::create(&renderdesc);

	g_game.obj=	load_mmod("model/box.mmod");
	g_game.sky= load_mmod("model/skyBOX.MMOD");

	for (unsigned n=0; n<g_game.sky->get_meshnum();++n)
	{
		mesh* m=g_game.sky->get_mesh(n);

		for (unsigned k=0; k<m->m_submeshbuf.size(); ++k)
		{
			submesh& sm=m->m_submeshbuf[k];

			sm.set_shader(shadermanager::ptr->get_shader("nothing.fx"));
		}
	}


	g_game.m_aspect=(float)renderdesc.m_screenwidth/(float)renderdesc.m_screenheight;

	::GetWindowRect(i_hwnd,&g_rect);
	g_time=timeGetTime();
//	g_hwnd=i_hwnd;

//	mtx4x4 mtx; mtx.set_projectionmatrix(tan(degreetorad(45)),g_game.m_aspect,1,10000);

	mesh* m=new mesh("fucka");

	vector<vertexelem> ve;

	ve.push_back(vertexelem(vertexelement_position,3));
	ve.push_back(vertexelem(vertexelement_normal,3));
	ve.push_back(vertexelem(vertexelement_uv,2));

	m->m_vb=new vertexbuffer(8,ve);
	m->m_ib=new indexbuffer(3*12);

	struct vertex_t
	{
		vec3 pos;
		vec3 normal;
		vec2 uv;
	};

	vertex_t* vb=(vertex_t*)m->m_vb->lock();

	for (int n=0; n<8; ++n)
	{
		vb[n].pos=BODY_SIZE*g_pos[n];
		vb[n].uv=g_uv[n];
		vb[n].normal=g_pos[n];  vb[n].normal.normalize();
	}


	m->m_vb->unlock();

	unsigned short* ib=(unsigned short*)m->m_ib->lock();

	const unsigned short buf[]={0,1,2,0,2,3,1,5,6,1,6,2,2,6,7,2,7,3,4,0,3,4,3,7,5,4,7,5,7,6,1,0,4,1,4,5};

	memcpy(ib,buf,36*sizeof(short));

	m->m_ib->unlock();


	m->m_submeshbuf.resize(1);
	submesh& ts=m->m_submeshbuf.back();
	ts.m_firstindex=0;
	ts.m_numindices=36;
	ts.m_firstvertex=0;
	ts.m_numvertices=8;

	ts.set_shader(shadermanager::ptr->get_shader("posnormuv.fx"));
	ts.bind_param("light_dir",&g_game.light_dir,3*sizeof(float));


//	texture* txt=texturemanager::instance()->get_texture("teszt.jpg");
	texture* txt=texturemanager::ptr->get_texture("structures_malayhouse.dds");
	ts.m_texturebuf.push_back(txt);

	g_game.x=g_game.y=g_game.z=0;

	g_game.m_mesh=m;


	vec3 pos[15000];
	int posnum;
	short index[16384];
	int indexnum;

	generate_sphere(pos,posnum,index,indexnum,BODY_SIZE,5);
	g_game.sphere=new mesh("sphere");
	g_game.sphere->m_vb=new vertexbuffer(posnum,ve);
	g_game.sphere->m_ib=new indexbuffer(indexnum*3);
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
	submesh& sts=g_game.sphere->m_submeshbuf.back();
	sts.m_firstindex=0;
	sts.m_numindices=indexnum*3;
	sts.m_firstvertex=0;
	sts.m_numvertices=posnum;

	sts.set_shader(shadermanager::ptr->get_shader("posnormuv.fx"));
	sts.bind_param("light_dir",&g_game.light_dir,3*sizeof(float));


	//	texture* txt=texturemanager::instance()->get_texture("teszt.jpg");
//	texture* txt=texturemanager::instance()->get_texture("white.bmp");
	sts.m_texturebuf.push_back(txt);


	bodydesc bd;
	bd.mass=1;
	bd.inertia.identity();
	bd.is_static=BODYSTATE_DYNAMIC;


	for (unsigned n=0; n<BODY_NUM;++n)
	{
		float x=random(-ROOM_SIZE,ROOM_SIZE);
		float y=0;//random(-ROOM_SIZE,ROOM_SIZE);
		float z=random(-ROOM_SIZE,ROOM_SIZE);
		bd.pos.t.set(x,y,z);
		x=random(-3.0f,3.0f);
		y=0;//random(-3.0f,3.0f);
		z=random(-3.0f,3.0f);
		bd.vel.set(x,y,z);
//		bd.rotvel.set(x/3,y/3,z/3);

		g_game.phb[n]=physicssystem::ptr->create_body(bd);
#if 0
		box_shape_desc sd;
		sd.pos.identity();
		sd.extent.set(BODY_SIZE,BODY_SIZE,BODY_SIZE);
#else
		sphere_shape_desc sd;
		sd.center.clear();
		sd.radius=BODY_SIZE;
#endif
		g_game.phb[n]->add_shape(sd);
	}

	convex_hull_desc hd;
	hd.face_thickness=.001f;
	vector<vec3>& b=hd.vertex_array;

#if 0
	b.resize(9);

	b[0].set(-1,0,-1);
	b[1].set(-1,0,1);
	b[2].set(-1,1,-1);
	b[3].set(-1,1,1);
	b[4].set(1,0,-1);
	b[5].set(1,0,1);
	b[6].set(1,1,-1);
	b[7].set(1,1,1);
	b[8].set(1,1.5f,1);
#else
#define buff_size 40
	b.resize(buff_size);

	for (unsigned n=0; n<buff_size; ++n)
		for (int m=0; m<3; ++m)
			b[n][m]=random(-1.0f, 1.0f);

#endif
	g_game.ch=generate_convex_hull(hd);



	g_game.inited=true;
}





void vec3_add(vec3& dst, const vec3& src)
{
    dst.x+=src.x;
    dst.y+=src.y;
    dst.z+=src.z;
}

void vec3_sub(vec3& dst, const vec3& src)
{
    dst.x-=src.x;
    dst.y-=src.y;
    dst.z-=src.z;
}

void vec3_mul_scalar(vec3& dst, float s)
{
    dst.x*=s;
    dst.y*=s;
    dst.z*=s;
}

void vec3_mul(vec3& dst, const vec3& src)
{
    dst.x*=src.x;
    dst.y*=src.y;
    dst.z*=src.z;
}


unsigned sumtime=0;

void update_app()
{
	timer update_time;
	char str[128];
	g_game.t.stop();
	float frame_time=g_game.t.get_seconds();
	g_game.t.reset();

	sprintf(str,"FPS:%.1d",(int)(1/frame_time));
	rendersystem::ptr->draw_text(800,10,color_f(1,1,0,1),str);
	taskmanager::ptr->flush();
	unsigned acttime=::timeGetTime();
	unsigned deltatime=min(acttime-g_time,100u);
	g_time=acttime;
	sumtime+=deltatime;
	float dt=(float)deltatime/1000.0f;

	if (sumtime>33)
	{
		inputsystem::ptr->Update();
		sumtime-=33;
	}

	timer t;
	t.reset();
//	physicssystem::ptr->simulate(dt);
	t.stop();
	unsigned sec=t.get_tick();
	sprintf(str,"simulation time:%d",sec);
	rendersystem::ptr->draw_text(10,10,color_f(1,1,1,1),str);

	sprintf(str,"pairnum:%d",physicssystem::ptr->broad_phase.pair_num);
	rendersystem::ptr->draw_text(10,40,color_f(1,1,1,1),str);

	//convex hull rajzolas
	for (unsigned n=0; n<g_game.ch.vertices.size()-1; ++n)
	{
		int firstadj=g_game.ch.vertices[n].adj_index;
		int adjnum=g_game.ch.vertices[n+1].adj_index-firstadj;

		vec3 start=g_game.ch.vertices[n].pos;

		for (int m=0; m<adjnum; ++m)
		{
			int vindex=g_game.ch.vertex_adjacency[firstadj+m];

			if (vindex<n)
			{
				vec3 end=g_game.ch.vertices[vindex].pos;
				rendersystem::ptr->draw_line(start,color_r8g8b8a8(0,0,0,255),end,color_r8g8b8a8(255,255,255,255));
			}
		}
	}

#if 0
	for (unsigned n=0; n<(unsigned)physicssystem::ptr->broad_phase.pair_num; ++n)
	{
		broadphasepair& pair=physicssystem::ptr->broad_phase.pair_array[n];

		{
			vec3 center=pair.object[0]->bounding_world.get_center();
			vec3 extent=pair.object[0]->bounding_world.get_extent();
			vec3 corner[8];

			const float x[]={-1,1,1,-1,-1,1,1,-1};

#if 0
			for (int n=0; n<8;++n)
			{
				corner[n].x=center.x+(x[n])*extent.x;
				corner[n].y=center.y+(2*((n&2)>>1)-1)*extent.y;
				corner[n].z=center.z+(2*((n&4)>>2)-1)*extent.z;
			}
#else
            for (int n=0; n<8; ++n)
            {
                corner[n]=extent;
                vec3 mul; mul.set(x[n],(2*((n&2)>>1)-1),(2*((n&4)>>2)-1));
                vec3_mul(corner[n],mul);
                vec3_add(corner[n],center);
            }
#endif
			int prev=3;
			for (int n=0; n<4;++n)
			{
//				rendersystem::ptr->draw_line(corner[n],color_r8g8b8a8(0,0,0,255),corner[prev],color_r8g8b8a8(255,255,255,255));
//				rendersystem::ptr->draw_line(corner[n+4],color_r8g8b8a8(0,0,0,255),corner[prev+4],color_r8g8b8a8(255,255,255,255));
//				rendersystem::ptr->draw_line(corner[n],color_r8g8b8a8(0,0,0,255),corner[n+4],color_r8g8b8a8(255,255,255,255));
				prev=n;
			}
		}
		{
			vec3 center=pair.object[1]->bounding_world.get_center();
			vec3 extent=pair.object[1]->bounding_world.get_extent();
			vec3 corner[8];

			const float x[]={-1,1,1,-1,-1,1,1,-1};

#if 0
			for (int n=0; n<8;++n)
			{
				corner[n].x=center.x+(x[n])*extent.x;
				corner[n].y=center.y+(2*((n&2)>>1)-1)*extent.y;
				corner[n].z=center.z+(2*((n&4)>>2)-1)*extent.z;
			}
#else
            for (int n=0; n<8; ++n)
            {
                corner[n]=extent;
                vec3 mul; mul.set(x[n],(2*((n&2)>>1)-1),(2*((n&4)>>2)-1));
                vec3_mul(corner[n],mul);
                vec3_add(corner[n],center);
            }
#endif

			int prev=3;

			for (int n=0; n<4;++n)
			{
//				rendersystem::ptr->draw_line(corner[n],color_r8g8b8a8(0,255,0,255),corner[prev],color_r8g8b8a8(0,255,0,255));
//				rendersystem::ptr->draw_line(corner[n+4],color_r8g8b8a8(0,255,0,255),corner[prev+4],color_r8g8b8a8(0,255,0,255));
//				rendersystem::ptr->draw_line(corner[n],color_r8g8b8a8(0,255,0,255),corner[n+4],color_r8g8b8a8(0,255,0,255));
				prev=n;
			}
		}

	}
#endif
	update_time.reset();
	inputsystem* ip=inputsystem::ptr;

	mtx4x3 cammtx=mtx4x3::identitymtx();
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

	mtx4x3 viewmtx; viewmtx.linearinvert(cammtx);

	rendersystem::ptr->set_projection_params(degreetorad(60),g_game.m_aspect,0.3f,10000,(mtx4x4)viewmtx);

	mtx4x3 mtx;
	mtx.set_euler(g_game.x,g_game.y,g_game.z);
	mtx.t.set(-1,0,2.5f);

	vec3 light_dir; light_dir.set(1,1,0);
	g_game.obj->get_worldposition().transformtransposed3x3(g_game.light_dir,light_dir);
	g_game.light_dir.normalize();

	g_game.x+=dt/10;
	g_game.y+=2*dt/10;
	g_game.z+=3*dt/10;

	for (unsigned n=0; n<BODY_NUM;++n)
	{
		mtx4x3 pos=g_game.phb[n]->get_pos();
		vec3 vel=g_game.phb[n]->get_vel();

		if (pos.t.x<-ROOM_SIZE)
		{
			pos.t.x=-ROOM_SIZE;
			vel.x=fabsf(vel.x);
		}
		if (pos.t.y<-ROOM_SIZE)
		{
			pos.t.y=-ROOM_SIZE;
			vel.y=fabsf(vel.y);
		}
		if (pos.t.z<-ROOM_SIZE)
		{
			pos.t.z=-ROOM_SIZE;
			vel.z=fabsf(vel.z);
		}

		if (pos.t.x>ROOM_SIZE)
		{
			pos.t.x=ROOM_SIZE;
			vel.x=-fabsf(vel.x);
		}
		if (pos.t.y>ROOM_SIZE)
		{
			pos.t.y=ROOM_SIZE;
			vel.y=-fabsf(vel.y);
		}
		if (pos.t.z>ROOM_SIZE)
		{
			pos.t.z=ROOM_SIZE;
			vel.z=-fabsf(vel.z);
		}

		g_game.phb[n]->set_pos(pos);
		g_game.phb[n]->set_vel(vel);

		for (unsigned m=0; m<1; ++m)
		{
			rendersystem::ptr->add_mesh(g_game.sphere.get(),pos);
			pos.t.x+=10;
		}
	}

/*
	rendersystem::ptr->add_mesh(g_game.m_mesh.get(),mtx);
	mtx.t.set(1,0,2.5f);
	rendersystem::ptr->add_mesh(g_game.sphere.get(),mtx);
	mtx.set_euler(0,0,0);
	mtx.t.set(0,0,22.5f);
	g_game.obj->set_worldposition(mtx);
//	g_game.obj->render();
*/
	g_game.sky->render();
	rendersystem::ptr->render();

	update_time.stop();
	unsigned update_tick=update_time.get_tick();

	sprintf(str,"UPT:%d",update_tick);
	rendersystem::ptr->draw_text(400,10,color_f(1,1,0,1),str);

}

void exit_app()
{
	g_game.inited=false;
	filesystem::release();
	taskmanager::release();
	g_game.m_mesh=NULL;
	g_game.sphere=NULL;
	delete g_game.obj;
	delete g_game.sky;
	shadermanager::release();
	texturemanager::release();
	inputsystem::release();
	rendersystem::release();
	physicssystem::release();
}

void generate_tetrahedron(vec3 o_pos[],float i_radius)
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

void generate_sphere(vec3 o_pos[],int& o_numvertices,short o_indices[],int& o_numfaces,float i_radius, int i_depth)
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
//	if (g_game.inited)
//		shadermanager::ptr->reload_shaders();
}
