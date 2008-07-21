#include "texturemanager.h"
#include "utils/misc.h"
#include "file/filesystem.h"
#include "render/rendersystem.h"

namespace render
{
	DEFINE_SINGLETON(texturemanager);

	texturemanager::texturemanager(const texturemanagerdesc* i_desc):
	m_texturegroup(i_desc->m_texturegroup)
	{
	}

	texture* texturemanager::get_texture(const char* i_texturename)
	{
		texture* t=m_map.get_data(i_texturename);

		if (t)
			return t;

		file::file texturefile;
		file::system::instance()->open_file(texturefile,m_texturegroup.c_str(),i_texturename,"rb");

		if (texturefile.opened())
		{
			char* buf=new char [texturefile.size()];
			texturefile.read_bytes(buf,texturefile.size());

			if (t=new texture(buf,texturefile.size(),i_texturename))
			{
				m_map.add_data(t);
			}
			delete [] buf;
		}
		else
		{
			utils::PRINT("a %s textura nincsen meg!\n",i_texturename);
		}
		return t;
	}

	void texturemanager::erase_texture(texture* t)
	{
		m_map.remove_data(t->get_name().c_str());
	}
}