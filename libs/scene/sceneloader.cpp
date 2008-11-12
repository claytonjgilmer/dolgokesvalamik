#include "sceneloader.h"
#include "nodefactory.h"
#include "node.h"
#include "containers/stringmap.h"

    struct mapelem
    {
        const string& get_name() const
        {
            return m_name;
        }
        vector<node*> m_child;
        string m_name;

        mapelem(const char* i_name):
        m_name(i_name)
        {
        }

        mapelem* Next;
    };

	node* sceneloader::load_scene(const char* i_scenename)
	{
		stringmap<mapelem,1024> map;
		vector<node*> nodebuf;
		vector<node*> root;

		lua Lua;
		Lua.InitState((lua::eLuaLib)(lua::LL_BASE | lua::LL_MATH));

		Lua.DoFile(i_scenename);

		lua::Variable scenetable=Lua.GetGlobalTable().GetVariable("Scene");

		assert(scenetable.IsTable());

		lua::Variable K,V;

		for (scenetable.Begin(K,V); !scenetable.End(K); scenetable.Next(K,V))
		{
			node* n =load_node(NULL,V);

			if (!n)
				continue;

			lua::Variable parentvar=V.GetVariable("Parent");

			if (parentvar.IsString())
			{
				string parentname=parentvar.GetString();
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

		assertion(root.size()==1);

		mapelem** ptr=map.get_buffer();

		for (unsigned n=0; n<1024; ++n)
		{
			mapelem* mapptr=ptr[n];

			while (mapptr)
			{
				mapptr->get_name();
				mapptr=mapptr->Next;
			}

		}

		assert(0);
		return NULL;
	}

	node* sceneloader::load_node(node* i_parent,lua::Variable& i_nodetable)
	{
		int exclude=i_nodetable.GetVariable("Exclude").GetBool(false);

		if (exclude)
			return NULL;

		string TypeName=i_nodetable.GetVariable("Type").GetString("");

		node* NewNode=(node*)metaobject_manager::create_object(TypeName.c_str());

		if (NewNode)
			NewNode->get_metaobject()->load_property(NewNode,i_nodetable);
		else
			return NULL;

		NewNode->on_load();

		if (i_parent)
			i_parent->add_child(NewNode);

		lua::Variable childtable=i_nodetable.GetVariable("Child");
		if (childtable.IsTable())
		{
			lua::Variable K,V;
			for (childtable.Begin(K,V); !childtable.End(K); childtable.Next(K,V))
			{
				load_node(NewNode,V);
			}
		}

		return NewNode;
	}
