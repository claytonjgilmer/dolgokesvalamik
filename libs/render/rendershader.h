#ifndef _rendershader_h_
#define _rendershader_h_

#include <d3dx9.h>
#include "base/resource.h"

namespace render
{
	class shader:public base::resource
	{
		friend class base::referencepointer<shader>;
	public:
		shader(const void* i_buf, unsigned i_bufsize, const char* i_name);
		shader* Next;

	private:
		~shader();
		LPD3DXEFFECT m_effect;
		ctr::string m_name;
	};
}
#endif//_rendershader_h_