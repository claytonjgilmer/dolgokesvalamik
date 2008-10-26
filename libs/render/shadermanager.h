#ifndef _shadermanager_h_
#define _shadermanager_h_

#include "utils/singleton.h"
#include "containers/string.h"
#include "containers/stringmap.h"
#include "render/shader.h"

	class shadermanagerdesc
	{
	public:
		string m_shadergroup;

		shadermanagerdesc(const char* i_shadergroup):
		m_shadergroup(i_shadergroup)
		{
		}
	};
	class shadermanager
	{
		DECLARE_SINGLETON_DESC(shadermanager,shadermanagerdesc);
	public:
		shadermanager(const shadermanagerdesc*);
		shader* get_shader(const char* i_shadername);
		void erase_shader(shader*);
		void reload_shaders();

	private:
		string m_shadergroup;
		stringmap<shader> m_map;
	};
#endif//_shadermanager_h_