#ifndef _rendershader_h_
#define _rendershader_h_

#include <d3dx9.h>
#include "utils/resource.h"

namespace render
{
	class shader:public utils::resource
	{
		friend class utils::referencepointer<shader>;
		friend struct shaderparameteradogato;

	public:
		shader(const void* i_buf, unsigned i_bufsize, const char* i_name);
		shader* Next;
		LPD3DXEFFECT m_effect;

	private:
		~shader();
		ctr::string m_name;
	};
}
#endif//_rendershader_h_