#ifndef _shader_h_
#define _shader_h_

#include <d3dx9.h>
#include "utils/resource.h"
#include "shaderparam.h"

	struct shader:resource
	{
		shader(const void* i_buf, unsigned i_bufsize, const char* i_name);
		void reload(const void* i_buf, unsigned i_bufsize);
		void set_constants(shader_param_array_t const & param_array);
		shader* Next;
		LPD3DXEFFECT m_effect;

		~shader();
	};
#endif//_shader_h_
