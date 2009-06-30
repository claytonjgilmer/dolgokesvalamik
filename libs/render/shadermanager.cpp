#include "shadermanager.h"
#include "utils/misc.h"
#include "file/filesystem.h"
#include "render/rendersystem.h"

	DEFINE_SINGLETON(shadermanager);

	shadermanager::shadermanager(const shadermanagerdesc* i_desc):
	m_shadergroup(i_desc->m_shadergroup)
	{
	}

	shader* shadermanager::get_shader(const char* i_shadername)
	{
		shader* t=m_map.get_data(i_shadername);

		if (t)
			return t;

		file_t shaderfile;
		filesystem::ptr->open_file(shaderfile,m_shadergroup.c_str(),i_shadername,"rb");

		if (shaderfile.opened())
		{
			char* buf=new char [shaderfile.size()+1];
			buf[shaderfile.size()]=0;
			shaderfile.read_bytes(buf,shaderfile.size());

			if (t=new shader(buf,shaderfile.size(),i_shadername))
			{
				m_map.add_data(t);
			}
			delete [] buf;
		}
		return t;
	}

	void shadermanager::erase_shader(shader* t)
	{
		m_map.remove_data(t->get_name().c_str());
	}

	void shadermanager::reload_shaders()
	{
		for (unsigned n=0; n<1024; ++n)
		{
			shader* ptr=m_map.get_buffer()[n];

			while (ptr)
			{
				file_t shaderfile;
				filesystem::ptr->open_file(shaderfile,m_shadergroup.c_str(),ptr->get_name().c_str(),"rb");

				if (shaderfile.opened())
				{
					char* buf=new char [shaderfile.size()+1];
					buf[shaderfile.size()]=0;
					shaderfile.read_bytes(buf,shaderfile.size());

					ptr->reload(buf,shaderfile.size());
					delete [] buf;
				}

				ptr=ptr->Next;
			}
		}
	}
