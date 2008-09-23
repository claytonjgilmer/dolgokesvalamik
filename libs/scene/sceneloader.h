#ifndef _sceneloader_h_
#define _sceneloader_h_

#include "scripting/lua.h"

namespace scene
{
	class node;

	class sceneloader
	{
	public:
		static node* load_scene(const char* i_scenename);
		static void save_scene(const char* i_scenename,node* i_scene);
	protected:
		static node* load_node(node* i_parent,scripting::lua::Variable& i_nodetable);
	};

}
#endif//_sceneloader_h_