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

	class material:public base::resource
	{
		friend class base::referencepointer<material>;
	public:
		material(const char* i_name);

		ctr::vector<base::referencepointer<texture> > m_texture;
		base::referencepointer<render::shader> m_shader;
//		base::colorf m_diffuse;
//		base::colorf m_ambient;

	private:
		~material(){}
	};
}
#endif//_rendermaterial_h_