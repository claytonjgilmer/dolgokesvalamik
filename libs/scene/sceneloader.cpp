#include "sceneloader.h"
#include "nodefactory.h"
#include "node.h"
#include "containers/stringmap.h"

namespace scene
{
	node* sceneloader::load_scene(const char* i_scenename)
	{
		struct mapelem
		{
			const ctr::string& get_name() const
			{
				return m_name;
			}
			ctr::vector<node*> m_child;
			ctr::string m_name;

			mapelem(const char* i_name):
			m_name(i_name)
			{
			}

			mapelem* Next;
		};

		ctr::stringmap<mapelem,1024> map;
		ctr::vector<node*> nodebuf;
		ctr::vector<node*> root;

		scripting::lua Lua;
		Lua.InitState((scripting::lua::eLuaLib)(scripting::lua::LL_BASE | scripting::lua::LL_MATH));

		Lua.DoFile(i_scenename);

		scripting::lua::Variable scenetable=Lua.GetGlobalTable().GetVariable("Scene");

		assert(scenetable.IsTable());

		scripting::lua::Variable K,V;

		for (scenetable.Begin(K,V); !scenetable.End(K); scenetable.Next(K,V))
		{
			node* n =load_node(NULL,V);

			scripting::lua::Variable parentvar=V.GetVariable("Parent");

			if (parentvar.IsString())
			{
				ctr::string parentname=parentvar.GetString();
				mapelem* pm=map.get_data(parentname.c_str());

				if (!pm)
				{
					pm=new mapelem(parentname.c_str());
					map.add_data(pm);
				}

				pm->m_child.push_back(n);
			}
			else
			{
				root.push_back(n);
			}

			nodebuf.push_back(n);
		}

		for (unsigned n=0; n<nodebuf.size(); ++n)
		{
			
		}

		assert(0);
		return NULL;
	}

	node* sceneloader::load_node(node* i_parent,scripting::lua::Variable& i_nodetable)
	{
		bool exclude=i_nodetable.GetVariable("Exclude").GetBool(false);

		if (exclude)
			return NULL;

		std::string TypeName=i_nodetable.GetVariable("Type").GetString("");

		node* NewNode=(node*)metaobject_manager::create_object(TypeName.c_str());

		if (NewNode)
			NewNode->get_metaobject()->load_property(NewNode,i_nodetable);
		else
			return NULL;

		if (i_parent)
			i_parent->add_child(NewNode);

		scripting::lua::Variable childtable=i_nodetable.GetVariable("Child");
		if (childtable.IsTable())
		{
			scripting::lua::Variable K,V;
			for (childtable.Begin(K,V); !childtable.End(K); childtable.Next(K,V))
			{
				load_node(NewNode,V);
			}
		}

		return NewNode;
	}
}