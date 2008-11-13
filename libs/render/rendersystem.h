/*
	betolt modellt, texturat (resourcemanagerbe)
	modellekbol lehet peldanyt kerni (klonozni)

*/
#ifndef _rendersystem_h_
#define _rendersystem_h_

#include "containers/listallocator.h"
#include "renderqueue.h"
#include "threading/taskmanager2.h"

#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "vertexelements.h"

#include "utils/singleton.h"
#include "math/mtx4x4.h"
#include <d3d9.h>
#include <d3dx9.h>

	struct color_f
	{
		float r,g,b,a;
		color_f(){}
		color_f(float i_r,float i_g,float i_b,float i_a):r(i_r),g(i_g),b(i_b),a(i_a){}
	};

	struct color_r8g8b8a8
	{
		uint8 r;
		uint8 g;
		uint8 b;
		uint8 a;

		color_r8g8b8a8()
		{
		}

		color_r8g8b8a8(uint8 i_r, uint8 i_g, uint8 i_b, uint8 i_a):
		r(i_r),
		g(i_g),
		b(i_b),
		a(i_a)
		{
		}


	};
	class rendersystemdesc
	{
	public:
		int					m_windowed;
		unsigned				m_backbuffercount;
		unsigned				m_screenwidth;
		unsigned				m_screenheight;
		HWND					m_window;

		rendersystemdesc()
		{
			m_windowed=true;
			m_backbuffercount=2;
			m_screenheight=768;
			m_screenwidth=1024;
			m_window=0;
		}
	};

	class texture;
	class state;

	class rendersystem
	{
		DECLARE_SINGLETON_DESC(rendersystem,rendersystemdesc);
	public:
		rendersystem(const rendersystemdesc*);
		~rendersystem();

		void set_renderstate(const state& i_state);

		void flush_queues();
		void render();

		void add_mesh(mesh* i_mesh, const mtx4x3& i_mtx, unsigned i_queueindex=0);

#define LINENUM_MAX		100000
		struct line_struct
		{
			vec3 start;
			color_r8g8b8a8 start_color;
			vec3 end;
			color_r8g8b8a8 end_color;
		};

		int draw_lines;
		int draw_texts;

		inline void draw_line(const vec3& i_start, const color_r8g8b8a8& i_start_color, const vec3& i_end, const color_r8g8b8a8& i_end_color)
		{
			if (draw_lines && lines.size()<LINENUM_MAX)
			{
				line_struct s;

				s.start=i_start;
				s.start_color=i_start_color;
				s.end=i_end;
				s.end_color=i_end_color;
				lines.push_back(s);
			}
		}

		vector<line_struct> lines;
		LPDIRECT3DVERTEXBUFFER9 line_buffer;

		struct str_struct
		{
			uint32 x;
			uint32 y;
			string str;
			color_f color;
		};

		vector<str_struct> strings;
		LPD3DXFONT font;
		LPD3DXSPRITE font_sprite;

		void draw_text(int x, int y,  color_f color,const char* str)
		{
			if (draw_texts)
			{
				str_struct s;
				s.x=x;
				s.y=y;
				s.str=str;
				s.color=color;
				strings.push_back(s);
			}
		}



		void init_queues(const vector<string> i_queuenames);

		void set_projection_params(float i_fov, float i_aspect, float i_nearz, float i_farz, const mtx4x4& i_viewmatrix);


		LPDIRECT3DDEVICE9 device() const;
	private:
		LPDIRECT3DDEVICE9 m_device;
		LPDIRECT3D9 m_sys;
		vector<renderqueue> m_queue;
		HWND m_window;

		member(mtx4x4,viewprojection_matrix);
		member(mtx4x4,view_matrix);
		member(mtx4x4,projection_matrix);
	};
#endif//_rendersystem_h_
