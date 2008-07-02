#ifndef _rendermaterial_h_
#define _rendermaterial_h_

#include "base/colorf.h"

namespace render
{
	class shader;
	class texture;

	class material
	{
	public:
		ctr::vector<texture*> m_texture;
		render::shader* m_shader;
//		base::colorf m_diffuse;
//		base::colorf m_ambient;
	};
}
#endif//_rendermaterial_h_