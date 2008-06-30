#ifndef _resourcemanager_h_
#define _resourcemanager_h_
#include "resource.h"

namespace base
{
	class resourcemanager
	{
	public:
		resourcemanager();
		virtual ~resourcemanager();

		void add_resource(resource* i_obj, const ctr::string& i_name);

	private:
	};
}
#endif//_resourcemanager_h_