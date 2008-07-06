#ifndef _texturemanager_h_
#define _texturemanager_h_

#include "base/singleton.h"
#include "containers/string.h"
#include "containers/stringmap.h"
#include "render/rendertexture.h"

namespace render
{
	class texturemanagerdesc
	{
	public:
		texturemanagerdesc(const char* i_texturegroup):
		m_texturegroup(i_texturegroup)
		{
		}

		ctr::string m_texturegroup;
	};
	class texturemanager
	{
		DECLARE_SINGLETON(texturemanager);
	public:
		texture* get_texture(const char* i_texturename);
		void erase_texture(texture*);

	private:
		ctr::string m_texturegroup;
		ctr::stringmap<texture> m_map;
	};
}
#endif//_texturemanager_h_