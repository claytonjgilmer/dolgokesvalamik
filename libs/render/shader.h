#ifndef _shader_h_
#define _shader_h_

#include <d3dx9.h>
#include "utils/resource.h"

namespace render
{
	class shader:public utils::resource
	{
	public:
		shader(const void* i_buf, unsigned i_bufsize, const char* i_name);
		void reload(const void* i_buf, unsigned i_bufsize);
		shader* Next;
		LPD3DXEFFECT m_effect;

	private:
		~shader();
	};
}
#endif//_shader_h_