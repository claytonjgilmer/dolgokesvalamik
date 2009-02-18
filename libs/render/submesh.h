#ifndef _submesh_h_
#define _submesh_h_

#include "shader.h"
#include "texture.h"
#include "containers/vector.h"
#include "utils/ref_ptr.h"
#include "shaderparam.h"

	struct submesh
	{
		submesh()
		{
		}

		submesh(shader* i_shader):
		m_shader(i_shader)
		{
		}

		void set_shader(shader* i_shader)
		{
			if (i_shader==m_shader.get())
				return;

			m_shader=i_shader;
		}

		void bind_param(const char* i_name, void* i_addr, unsigned i_size)
		{
			param_array.bind_param(i_name,i_addr,i_size);
		}

		void set_constants()
		{
			m_shader->set_constants(param_array);
		}



		unsigned m_firstvertex, m_numvertices, m_firstindex,m_numindices;

		vector<ref_ptr<texture> > m_texturebuf;
		ref_ptr<shader> m_shader;

		shader_param_array_t param_array;
	};
#endif//_submesh_h_
