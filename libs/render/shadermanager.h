#ifndef _shadermanager_h_
#define _shadermanager_h_

#include "base/singleton.h"
#include "containers/string.h"
#include "containers/stringmap.h"
#include "render/rendershader.h"

namespace render
{
	class shadermanagerdesc
	{
	public:
		ctr::string m_shadergroup;

		shadermanagerdesc(const char* i_shadergroup):
		m_shadergroup(i_shadergroup)
		{
		}
	};
	class shadermanager
	{
		DECLARE_SINGLETON(shadermanager);
	public:
		shader* get_shader(const char* i_shadername);
		void erase_shader(shader*);

	private:
		ctr::string m_shadergroup;
		ctr::stringmap<shader> m_map;
	};
}
#endif//_shadermanager_h_