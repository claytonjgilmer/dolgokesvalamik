/*
	betolt modellt, texturat (resourcemanagerbe)
	modellekbol lehet peldanyt kerni (klonozni)

*/
#ifndef _rendersystem_h_
#define _rendersystem_h_

#include "containers/listallocator.h"
#include "renderqueue.h"
#include "threading/taskmanager.h"

#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "vertexelements.h"

#include "utils/singleton.h"
#include "utils/color.h"
#include "math/mtx4x4.h"
#include <d3d9.h>
#include <d3dx9.h>

	struct rendersystemdesc
	{
		int					m_windowed;
		unsigned				m_backbuffercount;
		unsigned				m_screenwidth;
		unsigned				m_screenheight;
		HWND					m_window;
		color_r8g8b8a8		m_clear_color;

		rendersystemdesc()
		{
			m_clear_color.set(23,65,96,255);
			m_windowed=true;
			m_backbuffercount=2;
			m_screenheight=768;
			m_screenwidth=1024;
			m_window=0;
		}
	};

	struct texture;
	struct state;

	struct rendersystem
	{
		DECLARE_SINGLETON_DESC(rendersystem,rendersystemdesc);
		rendersystem(const rendersystemdesc*);
		~rendersystem();

		void set_renderstate(const state& i_state);

		void flush_queues();
		void render();

		void add_renderable(mesh_t* i_mesh, shader_param_array_t* param_array, const mtx4x3& i_mtx, unsigned i_queueindex=0);

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

		inline void draw_line(const vec3& i_start, const vec3& i_end, const color_r8g8b8a8& i_color)
		{
			if (draw_lines && lines.size()<LINENUM_MAX)
			{
				line_struct s;

				s.start=i_start;
				s.start_color=i_color;
				s.end=i_end;
				s.end_color=i_color;
				lines.push_back(s);
			}
		}

		inline void draw_line(const vec3& i_start, const vec3& i_end)
		{
			if (draw_lines && lines.size()<LINENUM_MAX)
			{
				line_struct s;

				s.start=i_start;
				s.start_color.set(255,255,255,255);
				s.end=i_end;
				s.end_color.set(255,255,255,255);
				lines.push_back(s);
			}
		}

		inline void draw_circle(const vec3& center, f32 radius, int section_count=10,const color_r8g8b8a8& i_color=color_r8g8b8a8(255,255,255,255))
		{
			vec3 x=m_view_matrix.column(0);
			vec3 y=m_view_matrix.column(1);

			vec3 start=center+radius*x;

			for (int n=0; n<section_count; ++n)
			{
				f32 angle=2*n*pi()/(section_count-1);
				vec3 end=center+cosf(angle)*radius*x+sinf(angle)*radius*y;
				draw_line(start,end,i_color);
				start=end;
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

		ref_ptr<texture> white_texture;



		void init_queues(const vector<string> i_queuenames);

		void set_projection_params(f32 i_fov, f32 i_aspect, f32 i_nearz, f32 i_farz, const mtx4x4& i_viewmatrix);


		LPDIRECT3DDEVICE9 device() const;
	private:
		LPDIRECT3DDEVICE9 m_device;
		LPDIRECT3D9 m_sys;
		vector<renderqueue> m_queue;
		HWND m_window;
		color_r8g8b8a8 clear_color;

		member(mtx4x4,viewprojection_matrix);
		member(mtx4x4,view_matrix);
		member(mtx4x4,projection_matrix);
	};
#endif//_rendersystem_h_
