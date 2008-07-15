#include "objectmanager.h"

namespace render
{
	objectmanager::objectmanager(const objectmanagerdesc* i_desc):
	m_objectgroup(i_desc->m_objectgroup)
	{
	}

	object3d* objectmanager::get_object(const char* i_name)
	{
		object3d* obj=m_map.get_data(i_name);

		if (obj)
			return obj;


	}


}