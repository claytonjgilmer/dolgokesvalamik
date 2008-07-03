#ifndef _rendershader_h_
#define _rendershader_h_

#include <d3dx9.h>
#include "base/resource.h"

namespace render
{
	class shader:public base::resource
	{
	public:
		shader(const ctr::string& i_name);
		~shader();

	private:
		LPD3DXEFFECT m_effect;
		ctr::string m_name;
	};
}
#endif//_rendershader_h_