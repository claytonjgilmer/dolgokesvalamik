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
#include "physics/collision/shapes/convexmeshdata.h"
#include "physics/collision/shapeintersection/satintersection.h"
#include "physics/collision/shapeintersection/gjkintersection.h"
#include "physics/collision/shapeintersection/deepintersection.h"
#include "math/geometry/intersection.h"
#include "utils/timer.h"

void draw_simplex(dvec3 s[], int num)
{
	for (int n=0; n<num; ++n)
		for (int m=n+1; m<num; ++m)
		{
			vec3 start; start.set((float)s[n].x,(float)s[n].y,(float)s[n].z);
			vec3 end; end.set((float)s[m].x,(float)s[m].y,(float)s[m].z);
			rendersystem::ptr->draw_line(start,color_r8g8b8a8(255,0,0,255),end,color_r8g8b8a8(255,0,0,255));
		}
}

const color_f color_black(0,0,0,1);
const color_f color_red(1,0,0,1);
const color_f color_green(0,1,0,1);
const color_f color_yellow(1,1,0,1);
const color_f color_blue(0,0,1,1);
const color_f color_purple(1,0,1,1);
const color_f color_cyan(0,1,1,1);
const color_f color_white(1,1,1,1);

color_f color_buf[]={color_black,color_red,color_green,color_yellow,color_blue,color_purple,color_cyan};

char* obj_names[]={
						"model/steamtrain.MMOD",
//						"model/sphere.MMOD",
//						"model/box.MMOD",
//						"model/Atlanta.MMOD",
//						"model/PT_Boat_Camo.MMOD",
//						"model/predator_spyplane.MMOD",
//						"model/f35_raptor.MMOD",
//						"model/ef2000.MMOD",
//						"model/Boeing747_jumbo.MMOD",
					};

int obj_count=sizeof(obj_names)/4;

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
			mesh_t* m=obj3d->get_mesh(n);
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

//void generate_sphere(vec3 o_pos[],int& o_numvertices,short o_indices[],int& o_numfaces,float i_radius, int i_depth);
object3d* load_mmod(const char* i_filename);
struct game
{
//	ref_ptr<mesh_t> m_mesh;
//	ref_ptr<mesh_t> sphere;
//	mtx4x3 m_mtx;

	float x,y,z;
	float camx,camy,camz;
	vec3 camt;
	float m_aspect;
	vec3 light_dir;
	vector<object3d*> obj;
	object3d* sky;
	object3d* terrain;
	object3d* sphere;
#define BODY_NUM 50
#define ROOM_SIZE 10.0f
#define BODY_SIZE .5f
	body_t* phb[BODY_NUM];
	object3d* model[BODY_NUM];
	int inited;
	timer_t t;

//	convex_hull ch;
	convex_hull_generator ch;
	convex_hull_desc hd;
	convex_mesh_data_t md;
	convex_mesh_data_t md2;

	vec3 obj_trans;

	game()
	{
		obj_trans.set(0,5,0);
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

void bind_light(node_t* node)
{
	if (node->get_metaobject()->isa(object3d::get_class_metaobject()->get_typeid()))
	{
		object3d* obj3d=(object3d*)node;

		for (unsigned n=0; n<obj3d->get_meshnum(); ++n)
		{
			mesh_t* mesh=obj3d->get_mesh(n);

			for (unsigned int m=0; m<mesh->m_submeshbuf.size(); ++m)
			{
				mesh->m_submeshbuf[m].bind_param("light_dir",&g_game->light_dir,3*sizeof(float));
			}
		}
	}

	for (node_t* child=node->get_child(); child; child=child->get_bro())
	{
		bind_light(child);
	}

}

#define frame_count 1000
unsigned timedeep[frame_count];
unsigned framedeep=0;
unsigned time1[frame_count];
unsigned frame1=0;
unsigned time2[frame_count];
unsigned frame2=0;
unsigned sumtime1=0;
unsigned sumtime2=0;
unsigned sumtimedeep=0;

void init_app(HWND i_hwnd)
{
	for (int n=0; n<frame_count; ++n)
		time1[n]=time2[n]=timedeep[n]=0;

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
	{
		g_game->obj.push_back(load_mmod(obj_names[n]));
		bind_light(g_game->obj.back());
	}

	g_game->sphere=load_mmod("model/sphere.mmod");
	bind_light(g_game->sphere);

	for (int n=0; n<BODY_NUM; ++n)
		g_game->model[n]=(object3d*)g_game->sphere->clone();

#if 0
	g_game->terrain=load_mmod("model/pearl_harbor.MMOD");
	mtx4x3 mtx=mtx4x3::identitymtx();
	mtx.t.y=-100;
	g_game->terrain->set_worldposition(mtx);
#else
	g_game->terrain=NULL;
#endif

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
		sd.radius=1;//BODY_SIZE;
#endif
		g_game->phb[n]->add_shape(sd);
	}

	g_game->hd.face_thickness=.00001;
	g_game->hd.vertex_min_dist=-.1;
	g_game->hd.triangle_output=false;


	g_game->hd.vertex_array.clear();
	get_object_vertices(g_game->obj[0],mtx4x3::identitymtx(),g_game->hd.vertex_array);
	g_game->ch.do_all(g_game->hd);
	hull_to_convex_mesh(g_game->md,g_game->ch.ch);
	hull_to_convex_mesh(g_game->md2,g_game->ch.ch);

	g_game->obj_trans.y=g_game->md.half_extent.y*2;


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

vec3 to_vec3(dvec3 v)
{
	vec3 r; r.set((float)v.x,(float)v.y,(float)v.z); return r;
}

//static double sinsum=0;
static double var=3.141592653589793238462643383279;
#define Pi 3.141592653589793238462643383279

double sajsin(double y)
{
//	if (y>Pi) y-=2*Pi;
//	else if (y<-Pi) y+=2*Pi;
	while (y>Pi) y-=2*Pi;
	while (y<-Pi) y+=2*Pi;
//	x=fmod(x,Pi);
#define B (4/Pi)
#define C (-4/(Pi*Pi))

//	double y = B * x + C * x * fabs(x);
	y*=(B  + C * fabs(y));

#if 1//def EXTRA_PRECISION
	//  const float Q = 0.775;
#define P 0.225

	y *= (P *(fabs(y) - 1) + 1);   // Q * y + P * y * abs(y)
#endif

	return y;
#undef B
#undef C
#undef P
}

void update_app()
{
	timer_t ttt;

#define itcount 100000
	ttt.reset();
	var=Pi/2;
	for (int n=0; n<itcount; ++n)
		var=(sin(2*var)*var+(sin(var)+sqr(sin(var/2))*2)*var)/2;

	ttt.stop();
	PRINT("%d  %f\n",ttt.get_tick(),var);


	var=Pi/2;
	ttt.reset();
	for (int n=0; n<itcount; ++n)
		var=(sajsin(2*var)+(sajsin(var)+sqr(sajsin(var/2))*2)*var)/2;

	ttt.stop();
	PRINT("%d  %f\n\n\n\n\n",ttt.get_tick(),var);
	PRINT("sin(pi)=%f,sin(pi/2)=%f,sin(pi/4)=%f,sin(0)=%f\n",sajsin(Pi),sajsin(Pi/2),sajsin(Pi/4),sajsin(0));
	PRINT("sin(pi)=%f,sin(pi/2)=%f,sin(pi/4)=%f,sin(0)=%f\n",sin(Pi),sin(Pi/2),sin(Pi/4),sin(0.0));




	timer_t update_time;
	char str[1024];
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
	physicssystem::ptr->simulate(dt);
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

	g_game->light_dir=-cammtx.z;

	float speed;

	if (ip->MouseButtonDown(0))
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
		int add=inputsystem::ptr->KeyDown(KEYCODE_LSHIFT) ? -1 : 1;
		obj_index=(obj_index+obj_count+add) % obj_count;
		g_game->hd.vertex_array.clear();
		get_object_vertices(g_game->obj[obj_index],mtx4x3::identitymtx(),g_game->hd.vertex_array);
		g_game->ch.do_all(g_game->hd);
		hull_to_convex_mesh(g_game->md,g_game->ch.ch);
		hull_to_convex_mesh(g_game->md2,g_game->ch.ch);
	}

	static float ex=0,ey=0,ez=0;

	if (inputsystem::ptr->KeyDown(KEYCODE_LSHIFT))
	{
		if (inputsystem::ptr->KeyDown(KEYCODE_UP))
			ex+=dt;
		if (inputsystem::ptr->KeyDown(KEYCODE_DOWN))
			ex-=dt;
		if (inputsystem::ptr->KeyDown(KEYCODE_LEFT))
			ey-=dt;
		if (inputsystem::ptr->KeyDown(KEYCODE_RIGHT))
			ey+=dt;
	}
	else
	{
		if (inputsystem::ptr->KeyDown(KEYCODE_UP))
			g_game->obj_trans.y+=dt;
		if (inputsystem::ptr->KeyDown(KEYCODE_DOWN))
			g_game->obj_trans.y-=dt;
		if (inputsystem::ptr->KeyDown(KEYCODE_LEFT))
			g_game->obj_trans.z-=dt;
		if (inputsystem::ptr->KeyDown(KEYCODE_RIGHT))
			g_game->obj_trans.z+=dt;
		if (inputsystem::ptr->KeyDown(KEYCODE_NUMPAD4))
			g_game->obj_trans.x-=dt;
		if (inputsystem::ptr->KeyDown(KEYCODE_NUMPAD6))
			g_game->obj_trans.x+=dt;
	}



	if (0)// || !keszvan)
		g_game->ch.draw(g_game->obj[obj_index]->get_worldposition());
	else
	{
//		draw_hull(&g_game->ch.ch,g_game->obj[obj_index]->get_worldposition());
		mtx4x3 mtx1=g_game->obj[obj_index]->get_worldposition();
		mtx1.set_euler(ex,ey,ez);
		mtx4x3 mtx2=mtx4x3::identitymtx(); 
		mtx2.t=mtx1.t+g_game->obj_trans;
		g_game->obj[obj_index]->set_worldposition(mtx1);

		static bool deep_init=false;
		static vec3 deep_init_vec=to_vec3(0,0,0);
		static edge_data* deep_init_v1=NULL;
		static edge_data* deep_init_v2=NULL;
		static int deep_init_state=0;

		if (inputsystem::ptr->KeyPressed(KEYCODE_F))
		{
			deep_init=!deep_init;
			deep_init_vec=to_vec3(0,0,0);
		}


		timer_t t;
		t.reset();
		deep_intersection deep(&g_game->ch.ch,&g_game->ch.ch,mtx1,mtx2,deep_init_vec,deep_init_v1,deep_init_v2,deep_init_state);
		t.stop();

		if (deep_init && deep.result)
		{
			deep_init_vec=deep.normal; deep_init_vec.normalize();
			deep_init_v1=deep.v1;
			deep_init_v2=deep.v2;
			deep_init_state=deep.state;
		}

		sumtimedeep-=timedeep[frame1];
		timedeep[framedeep]=t.get_tick();
		sumtimedeep+=timedeep[framedeep];
		framedeep=(framedeep+1) % frame_count;

		sprintf(str,"deeptime:%d iter:%d  init:%d",sumtimedeep/frame_count,deep.itnum,deep_init);
		rendersystem::ptr->draw_text(50,130,color_f(1,1,1,1),str);

		t.reset();
		vec3 pen;
		vec3 center=g_game->ch.ch.center;
		vec3 extent=g_game->ch.ch.half_extent;
		uint32 res=obb_obb_intersect(pen,center,extent,mtx1,center,extent,mtx2);
		t.stop();
		sprintf(str,"boxboxtime:%d,result:%d",t.get_tick(),res);
		rendersystem::ptr->draw_text(50,150,color_f(1,1,1,1),str);
		t.reset();
		res=sphere_sphere_intersect(pen,mtx1.t,extent.x,mtx2.t,extent.x);
		t.stop();
		sprintf(str,"sphsphtime:%d,result:%d",t.get_tick(),res);
		rendersystem::ptr->draw_text(50,170,color_f(1,1,1,1),str);

		if (res)
			rendersystem::ptr->draw_circle(mtx2.t+pen,extent.x,30,color_r8g8b8a8(255,0,255,0));



		dvec3 initdir;
		{
			t.reset();
			gjk_intersection gjk(&g_game->md,&g_game->md2,mtx1,mtx2);
			t.stop();
			sumtime1-=time1[frame1];
			time1[frame1]=t.get_tick();
			sumtime1+=time1[frame1];
			frame1=(frame1+1) % frame_count;
			float vol=0;
			if (gjk.simplex_size==4)
				vol=get_tetrahedron_volume(to_vec3(gjk.simplex[0]),to_vec3(gjk.simplex[1]),to_vec3(gjk.simplex[2]),to_vec3(gjk.simplex[3]));
			sprintf(str,"gjk time:%03d, result:%d, out:%d, dir:(%g %g %g),prevdir: (%g %g %g), simplex:%d, vol:%.1f, iter:%d",
				sumtime1/frame_count, gjk.result,gjk.out,gjk.dir.x, gjk.dir.y,gjk.dir.z,gjk.prevdir.x, gjk.prevdir.y,gjk.prevdir.z,gjk.simplex_size,vol,gjk.iteration);
			rendersystem::ptr->draw_text(50,90,color_f(1,1,1,1),str);
			initdir=gjk.prevdir;
		}
		{
			t.reset();
			gjk_intersection gjk(&g_game->md,&g_game->md2,mtx1,mtx2,initdir);
			t.stop();
			float vol=0;
			if (gjk.simplex_size==4)
				vol=get_tetrahedron_volume(to_vec3(gjk.simplex[0]),to_vec3(gjk.simplex[1]),to_vec3(gjk.simplex[2]),to_vec3(gjk.simplex[3]));
			sprintf(str,"gjk time:%03d, result:%d, out:%d, dir:(%g %g %g),prevdir: (%g %g %g), simplex:%d, vol:%.1f, iter:%d",
				t.get_tick(), gjk.result,gjk.out,gjk.dir.x, gjk.dir.y,gjk.dir.z,gjk.prevdir.x, gjk.prevdir.y,gjk.prevdir.z,gjk.simplex_size,vol,gjk.iteration);
			rendersystem::ptr->draw_text(50,110,color_f(1,1,1,1),str);
//		g_game->md.ph.draw(g_game->md.vert,mtx1);
//		g_game->md2.ph.draw(g_game->md.vert,mtx2);

		//		draw_hull(&g_game->md,mtx1);
		draw_hull(&g_game->md2,mtx2);

		if (deep.result)
		{
			mtx4x3 mtx3=mtx2;
			mtx3.t+=deep.normal;
			draw_hull(&g_game->md2,mtx3);
		}
		draw_simplex(gjk.simplex,gjk.simplex_size);
		}
		{
			t.reset();
			gjk_intersection2 gjk(&g_game->ch.ch,&g_game->ch.ch,mtx1,mtx2);//,initdir);
			t.stop();
			sumtime2-=time2[frame2];
			time2[frame2]=t.get_tick();
			sumtime2+=time2[frame2];
			frame2=(frame2+1) % frame_count;
			float vol=0;
			if (gjk.simplex_size==4)
				vol=get_tetrahedron_volume(to_vec3(gjk.simplex[0]),to_vec3(gjk.simplex[1]),to_vec3(gjk.simplex[2]),to_vec3(gjk.simplex[3]));
			sprintf(str,"gjk2time:%03d, result:%d, out:%d, dir:(%g %g %g),prevdir: (%g %g %g), simplex:%d, vol:%.1f, iter:%d",
				sumtime2/frame_count, gjk.result,gjk.out,gjk.dir.x, gjk.dir.y,gjk.dir.z,gjk.prevdir.x, gjk.prevdir.y,gjk.prevdir.z,gjk.simplex_size,vol,gjk.iteration);
			rendersystem::ptr->draw_text(50,70,color_f(1,1,1,1),str);
		}

	}





	mtx4x3 viewmtx; viewmtx.linearinvert(cammtx);

	rendersystem::ptr->set_projection_params(degreetorad(60),g_game->m_aspect,0.3f,10000,(mtx4x4)viewmtx);

	mtx4x3 mtx;
	mtx.set_euler(g_game->x,g_game->y,g_game->z);
	mtx.t.set(-1,0,2.5f);

//	g_game->light_dir.set(1,1,0);
//	g_game->obj[0]->get_worldposition().transformtransposed3x3(g_game->light_dir,light_dir);
//	g_game->light_dir.normalize();

	g_game->x+=dt/10;
	g_game->y+=2*dt/10;
	g_game->z+=3*dt/10;

	if (1)
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

		g_game->model[n]->set_worldposition(pos);

		if (g_game->phb[n]->group_index==-1)
			g_game->model[n]->color=color_f(1,1,1,1);
		else
			g_game->model[n]->color=color_buf[g_game->phb[n]->group_index % array_elemcount(color_buf)];
		g_game->model[n]->render();
	}





	mtx=g_game->obj[0]->get_worldposition();
	for (unsigned n=0; n<g_game->obj.size(); ++n)
	{
		g_game->obj[n]->set_worldposition(mtx);
		g_game->obj[n]->render();
		mtx.t.x+=25;
	}

	if (g_game->terrain)
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

	delete g_game->sphere;

	for (int n=0; n<BODY_NUM; ++n)
		delete g_game->model[n];

	if (g_game->terrain)
		delete g_game->terrain;

	shadermanager::release();
	inputsystem::release();
	rendersystem::release();
	texturemanager::release();
	physicssystem::release();

	delete g_game;
}

void generate_tetrahedron(vec3 o_pos[],float i_radius)
{
//	float Pi = 3.141592653589793238462643383279502884197f;

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
