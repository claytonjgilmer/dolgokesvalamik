#include "objectmanager.h"
#include "file/file.h"
#include "file/filesystem.h"

object3d* load_mmod(file_t& i_file);
	DEFINE_SINGLETON(objectmanager);

	objectmanager::objectmanager(const objectmanagerdesc* i_desc):
	m_objectgroup(i_desc->m_objectgroup)
	{
	}

	object3d* objectmanager::get_object(const char* i_name)
	{
		object3d* obj=m_map.get_data(i_name);

//		if (obj)
//			return obj->clone();

		string ext=get_extension(i_name);
		ext.to_upper();

		if (ext=="MMOD")
		{
			obj=load_mmod_file(i_name);
		}

		if (obj)
		{
			m_map.add_data(obj);
		}

		node_t* ptr=obj;

	//	ptr=ptr->get_next(obj);

		while (ptr)
		{
//			ptr->set_flag(nodeflag_dont_save);
			ptr=ptr->get_next(obj);
		}

		return obj;

	}

	object3d* objectmanager::load_mmod_file(const char* i_name)
	{
		file_t objfile;
		filesystem::ptr->open_file(objfile,m_objectgroup.c_str(),i_name,"rb");

		if (objfile.opened())
		{
			object3d* obj=load_mmod(objfile);

			if (obj)
				obj->set_name(i_name);

			return obj;
		}


		return NULL;
	}
