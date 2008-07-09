#ifndef _rendermaterial_h_
#define _rendermaterial_h_

#include "utils/colorf.h"
#include "utils/resource.h"
#include "utils/referencepointer.h"
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

		ctr::vector<utils::referencepointer<texture> > m_texturebuf;
		utils::referencepointer<render::shader> m_shader;
	};
}
#endif//_rendermaterial_h_