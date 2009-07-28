#ifndef _texturemanager_h_
#define _texturemanager_h_

#include "utils/singleton.h"
#include "containers/string.h"
#include "containers/stringmap.h"
#include "render/texture.h"

	struct texturemanagerdesc
	{
		texturemanagerdesc():
		m_texturegroup("texture")
		{
		}
		texturemanagerdesc(const char* i_texturegroup):
		m_texturegroup(i_texturegroup)
		{
		}

		string m_texturegroup;
	};

	struct texturemanager
	{
		DECLARE_SINGLETON_DESC(texturemanager,texturemanagerdesc);
		texturemanager(const texturemanagerdesc* i_desc);
		texture* get_texture(const char* i_texturename);
		void erase_texture(texture*);

		string m_texturegroup;
		stringmap<texture> m_map;
	};
#endif//_texturemanager_h_
