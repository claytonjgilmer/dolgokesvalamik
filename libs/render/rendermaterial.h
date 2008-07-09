#ifndef _rendermaterial_h_
#define _rendermaterial_h_

#include "base/colorf.h"
#include "base/resource.h"
#include "base/referencepointer.h"
#include "containers/vector.h"

namespace render
{
	class shader;
	class texture;

	class material
	{
	public:
		material(){}
		~material(){}

		ctr::vector<base::referencepointer<texture> > m_texturebuf;
		base::referencepointer<render::shader> m_shader;
	};
}
#endif//_rendermaterial_h_