#ifndef _rendersubmesh_h_
#define _rendersubmesh_h_

#include "rendershader.h"
#include "rendertexture.h"
#include "containers/vector.h"

namespace render
{
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
			if (i_shader==m_shader)
				return;

			m_shader=i_shader;
			m_numparams=0;
		}

		void bind_param(const char* i_name, void* i_addr, unsigned i_size)
		{
			utils::assertion(m_numparams<20,"tul sok a shader parametere");

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

		ctr::vector<utils::referencepointer<texture>> m_texturebuf;
		utils::referencepointer<shader> m_shader;

		struct param
		{
			ctr::string m_name;
			void* m_addr;
			unsigned m_size;
		};

		param m_paramvec[40];
		unsigned m_numparams;
	};
}
#endif//_rendersubmesh_h_