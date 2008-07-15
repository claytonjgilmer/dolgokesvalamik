#include "objectmanager.h"
#include "file/file.h"
#include "file/filesystem.h"

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

		ctr::string ext=file::get_extension(i_name);
		ext.to_upper();

		if (ext=="MMOD")
		{
			return load_mmod_file(i_name);
		}

		return NULL;

	}

	object3d* objectmanager::load_mmod_file(const char* i_name)
	{
		file::file objfile;
		file::system::instance()->open_file(objfile,i_name,m_objectgroup.c_str(),"rb");

		return NULL;
	}
}