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
#include "math/geometry/convexhull.h"
#include "math/geometry/convexhullgeneration.h"
#include "utils/timer.h"

char* obj_names[]={
						"model/predator_spyplane.MMOD",
						"model/f35_raptor.MMOD",
						"model/ef2000.MMOD",
						"model/Boeing747_jumbo.MMOD",
					};

int obj_count=sizeof(obj_names)/4;

vec3 to_vec3(float x,float y, float z)
{
	vec3 v; v.set(x,y,z);
	return v;
}

void draw_grid(const vec3& campos, float gridsize, int cellnum)
{
	float camx=floorf(campos.x/gridsize)*gridsize;
	float camz=floorf(campos.z/gridsize)*gridsize;


	for (int n=-cellnum; n<cellnum; ++n)
	{
		for (int m=-cellnum; m<cellnum; ++m)
		{
			float actx=camx+n*gridsize;
			float actz=camz+m*gridsize;
			rendersystem::ptr->draw_line(to_vec3(actx,0,actz),color_r8g8b8a8(0,0,64,255),to_vec3(actx+gridsize,0,actz),color_r8g8b8a8(0,0,64,255));
			rendersystem::ptr->draw_line(to_vec3(actx,0,actz),color_r8g8b8a8(0,0,64,255),to_vec3(actx,0,actz+gridsize),color_r8g8b8a8(0,0,64,255));
		}
	}
}

void get_object_vertices(node_t* obj, const mtx4x3& mtx, vector<vec3>& vertbuf)
{
	if (obj->get_metaobject()->isa(object3d::get_class_metaobject()->get_typeid()))
	{
		object3d* obj3d=(object3d*)obj;

		for (unsigned n=0; n<obj3d->get_meshnum(); ++n)
		{
			mesh* m=obj3d->get_mesh(n);
			char* v=(char*)m->m_vb->lock();

			for (unsigned j=0; j<m->m_vb->m_vertexnum; ++j)
			{
				vertbuf.push_back(mtx.transform(*(vec3*)v));
				v+=m->m_vb->m_vertexsize;
			}
		}
	}

	for (node_t* child=obj->get_child(); child; child=child->get_bro())
	{
		mtx4x3 cmtx; cmtx.multiply(child->get_localposition(),mtx);
		get_object_vertices(child,cmtx,vertbuf);
	}
}

void generate_sphere(vec3 o_pos[],int& o_numvertices,short o_indices[],int& o_numfaces,float i_radius, int i_depth);
object3d* load_mmod(const char* i_filename);
struct game
{
//	ref_ptr<mesh> m_mesh;
//	ref_ptr<mesh> sphere;
//	mtx4x3 m_mtx;

	float x,y,z;
	float camx,camy,camz;
	vec3 camt;
	float m_aspect;
	vec3 light_dir;
	vector<object3d*> obj;
	object3d* sky;
	object3d* terrain;
#define BODY_NUM 50
#define ROOM_SIZE 10.0f
#define BODY_SIZE .5f
	body_t* phb[BODY_NUM];
	int inited;
	timer_t t;

//	convex_hull ch;
	convex_hull_generator ch;
	convex_hull_desc hd;

	game()
	{
		camz=camy=camx=0;
		camt.set(0,0,-10);
		inited=false;
		t.reset();
	}
};

game* g_game=NULL;

void change_screen_size(unsigned i_newwidth, unsigned i_newheight)
{
	if (!i_newheight || !i_newwidth)
		return;

	if (g_game)
		g_game->m_aspect=i_newwidth/(float)i_newheight;
}


RECT g_rect;
unsigned g_time;
//HWND g_hwnd;

#define V vec3
#define V2 vec2

vec3 g_pos[8]={{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};
vec2 g_uv[]={V2(0,0),V2(1,0),V2(1,1),V2(0,1),V2(1,1),V2(0,1),V2(0,0),V2(1,0)};

static int obj_index=0;

void init_app(HWND i_hwnd)
{
	g_game=new game;
	filesystem::create();
	filesystem::ptr->register_path("shader","shader\\");
	filesystem::ptr->register_path("texture","texture\\");

	taskmanagerdesc tdesc;
	tdesc.m_threadnum=4;
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
	renderdesc.m_clear_color.set(64,64,255,0);

	rendersystem::create(&renderdesc);

	for (int n=0; n<obj_count; ++n)
		g_game->obj.push_back(load_mmod(obj_names[n]));
	g_game->terrain= load_mmod("model/pearl_harbor.MMOD");
	mtx4x3 mtx=mtx4x3::identitymtx();
	mtx.t.y=-100;
	g_game->terrain->set_worldposition(mtx);
/*
	for (unsigned n=0; n<g_game->terrain->get_meshnum();++n)
	{
		mesh* m=g_game->sky->get_mesh(n);

		for (unsigned k=0; k<m->m_submeshbuf.size(); ++k)
		{
			submesh& sm=m->m_submeshbuf[k];

			sm.set_shader(shadermanager::ptr->get_shader("nothing.fx"));
		}
	}
*/

	g_game->m_aspect=(float)renderdesc.m_screenwidth/(float)renderdesc.m_screenheight;

	::GetWindowRect(i_hwnd,&g_rect);
	g_time=timeGetTime();
//	g_hwnd=i_hwnd;

//	mtx4x4 mtx; mtx.set_projectionmatrix(tan(degreetorad(45)),g_game->m_aspect,1,10000);


	g_game->x=g_game->y=g_game->z=0;





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

		g_game->phb[n]=physicssystem::ptr->create_body(bd);
#if 0
		box_shape_desc sd;
		sd.pos.identity();
		sd.extent.set(BODY_SIZE,BODY_SIZE,BODY_SIZE);
#else
		sphere_shape_desc sd;
		sd.center.clear();
		sd.radius=BODY_SIZE;
#endif
		g_game->phb[n]->add_shape(sd);
	}

	g_game->hd.face_thickness=.00001;
	g_game->hd.triangle_output=false;


	g_game->hd.vertex_array.clear();
	get_object_vertices(g_game->obj[0],mtx4x3::identitymtx(),g_game->hd.vertex_array);
	g_game->ch.do_all(g_game->hd);

	g_game->inited=true;
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
int manual=false;

void update_app()
{
	timer_t update_time;
	char str[128];
	g_game->t.stop();
	float frame_time=g_game->t.get_seconds();
	g_game->t.reset();

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
		inputsystem::ptr->Update(true);
		sumtime-=33;
	}
	else
		inputsystem::ptr->Update(false);

	timer_t t;
	t.reset();
//	physicssystem::ptr->simulate(dt);
	t.stop();
	unsigned sec=t.get_tick();
	sprintf(str,"simulation time:%d",sec);
	rendersystem::ptr->draw_text(10,10,color_f(1,1,1,1),str);

	sprintf(str,"pairnum:%d",physicssystem::ptr->broad_phase.pair_num);
	rendersystem::ptr->draw_text(10,40,color_f(1,1,1,1),str);

	//convex hull rajzolas
	update_time.reset();
	inputsystem* ip=inputsystem::ptr;

	mtx4x3 cammtx=mtx4x3::identitymtx();
	cammtx.set_euler(g_game->camx,g_game->camy,g_game->camz);

	float speed;

	if (ip->KeyDown(KEYCODE_LSHIFT))
		speed=20;
	else
		speed=3;

	if (ip->KeyDown(KEYCODE_W))
		g_game->camt+=speed*dt*cammtx.z;

	if (ip->KeyDown(KEYCODE_S))
		g_game->camt-=speed*dt*cammtx.z;

	if (ip->KeyDown(KEYCODE_A))
		g_game->camt-=speed*dt*cammtx.x;

	if (ip->KeyDown(KEYCODE_D))
		g_game->camt+=speed*dt*cammtx.x;

	if (ip->KeyDown(KEYCODE_Q))
		g_game->camt+=speed*dt*cammtx.y;

	if (ip->KeyDown(KEYCODE_Z))
		g_game->camt-=speed*dt*cammtx.y;

	int mx=ip->GetMouseX();
	int my=ip->GetMouseY();


	g_game->camy+=dt*(float)ip->GetMouseX()/10.0f;
	g_game->camx+=dt*(float)ip->GetMouseY()/10.0f;

	cammtx.t=g_game->camt;
	draw_grid(g_game->camt,20,20);

#if 0
	static bool keszvan=false;

	if (!keszvan)
	{
		if (inputsystem::ptr->KeyPressed(KEYCODE_SPACE))
		{
			if (manual)
				keszvan=g_game->ch.generate();
			else
			while (!keszvan && !manual)
				keszvan=g_game->ch.generate();
		}

		if (keszvan)
			g_game->ch.get_result();
	}
#endif

	if (inputsystem::ptr->KeyPressed(KEYCODE_SPACE))
	{
		obj_index=(obj_index+1) % obj_count;
		g_game->hd.vertex_array.clear();
		get_object_vertices(g_game->obj[obj_index],mtx4x3::identitymtx(),g_game->hd.vertex_array);
		g_game->ch.do_all(g_game->hd);
	}


	if (1)// || !keszvan)
		g_game->ch.draw(g_game->obj[obj_index]->get_worldposition());
	else
	{
		draw_hull(&g_game->ch.ch);
	}





	mtx4x3 viewmtx; viewmtx.linearinvert(cammtx);

	rendersystem::ptr->set_projection_params(degreetorad(60),g_game->m_aspect,0.3f,10000,(mtx4x4)viewmtx);

	mtx4x3 mtx;
	mtx.set_euler(g_game->x,g_game->y,g_game->z);
	mtx.t.set(-1,0,2.5f);

	vec3 light_dir; light_dir.set(1,1,0);
	g_game->obj[0]->get_worldposition().transformtransposed3x3(g_game->light_dir,light_dir);
	g_game->light_dir.normalize();

	g_game->x+=dt/10;
	g_game->y+=2*dt/10;
	g_game->z+=3*dt/10;

	if (0)
	for (unsigned n=0; n<BODY_NUM;++n)
	{
		mtx4x3 pos=g_game->phb[n]->get_pos();
		vec3 vel=g_game->phb[n]->get_vel();

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

		g_game->phb[n]->set_pos(pos);
		g_game->phb[n]->set_vel(vel);

	}

/*
	rendersystem::ptr->add_mesh(g_game->m_mesh.get(),mtx);
	mtx.t.set(1,0,2.5f);
	rendersystem::ptr->add_mesh(g_game->sphere.get(),mtx);
	*/
	mtx.set_euler(0,0,0);
	mtx.t.set(0,0,0);

	for (unsigned n=0; n<g_game->obj.size(); ++n)
	{
		g_game->obj[n]->set_worldposition(mtx);
		g_game->obj[n]->render();
		mtx.t.x+=25;
	}
	g_game->terrain->render();
	rendersystem::ptr->render();

	update_time.stop();
	unsigned update_tick=update_time.get_tick();

	sprintf(str,"UPT:%d",update_tick);
	rendersystem::ptr->draw_text(400,10,color_f(1,1,0,1),str);

}

void exit_app()
{
	g_game->inited=false;
	filesystem::release();
	taskmanager::release();

	for (unsigned n=0; n<g_game->obj.size(); ++n)
		delete g_game->obj[n];
	delete g_game->terrain;
	shadermanager::release();
	texturemanager::release();
	inputsystem::release();
	rendersystem::release();
	physicssystem::release();

	delete g_game;
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
//	if (g_game->inited)
//		shadermanager::ptr->reload_shaders();
}
