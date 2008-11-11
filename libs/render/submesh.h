#ifndef _submesh_h_
#define _submesh_h_

#include "shader.h"
#include "texture.h"
#include "containers/vector.h"
#include "utils/ref_ptr.h"

	struct submesh
	{
		submesh():m_numparams(0)
		{
		}

		submesh(shader* i_shader):
		m_shader(i_shader),
			m_numparams(0)
		{
		}

		void set_shader(shader* i_shader)
		{
			if (i_shader==m_shader.get())
				return;

			m_shader=i_shader;
			m_numparams=0;
		}

		void bind_param(const char* i_name, void* i_addr, unsigned i_size)
		{
			assertion(m_numparams<20,"tul sok a shader parametere");

			m_paramvec[m_numparams].m_name=i_name;
			m_paramvec[m_numparams].m_addr=i_addr;
			m_paramvec[m_numparams].m_size=i_size;

			//			m_parammap.add_data(m_paramvec+m_numparams);
			++m_numparams;
		}

		void set_constants()
		{
			for (unsigned n=0; n<m_numparams;++n)
				m_shader->m_effect->SetValue(m_paramvec[n].m_name.c_str(),m_paramvec[n].m_addr,m_paramvec[n].m_size);
		}



		unsigned m_firstvertex, m_numvertices, m_firstindex,m_numindices;

		vector<ref_ptr<texture> > m_texturebuf;
		ref_ptr<shader> m_shader;

		struct param
		{
			string m_name;
			void* m_addr;
			unsigned m_size;
		};

		param m_paramvec[40];
		unsigned m_numparams;
	};
#endif//_submesh_h_
