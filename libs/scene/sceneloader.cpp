#include "sceneloader.h"
#include "nodefactory.h"
#include "node.h"
#include "containers/stringmap.h"
#include "file/file.h"

node_t* load_node(json_object_t* obj)
{
	metaobject* mo=NULL;
	for (unsigned n=0; n<obj->pair_list.size(); ++n)
	{
		if (_stricmp(obj->pair_list[n].key,"type") == 0)
		{
			assertion(obj->pair_list[n].val.is_str());
			mo=metaobject_manager::get_metaobject(obj->pair_list[n].val.get_str());
			break;
		}
	}

	if (!mo)
		return NULL;
	assertion(mo!=NULL);

	node_t* node=(node_t*)mo->create();
	mo->load_properties(node,obj);
	node->on_load();

	for (unsigned n=0; n<obj->pair_list.size(); ++n)
	{
		if (_stricmp(obj->pair_list[n].key,"child") == 0)
		{
			assertion( obj->pair_list[n].val.is_array());
			json_array_t* child_array=obj->pair_list[n].val.arr;

			for (unsigned m=0; m<child_array->value_list.size(); ++m)
			{
				assertion(child_array->value_list[m].is_object());
				node_t* child_node=load_node(child_array->value_list[m].obj);

				if (!child_node)
					continue;
				node->add_child(child_node);
			}
			break;
		}
	}
	return node;
//	mo->load_property()
}

node_t* load_scene(const char* i_scenename)
{
	file_t file(i_scenename,"rt");

	if (file.opened())
	{
		unsigned filesize=file.size();
		char* buf=new char[filesize+1];
		file.read_bytes(buf,filesize);
		buf[filesize]=0;
		file.close();

		json_map_t* map=generate_json_map(buf);

		assertion(map->item_array.size()==1 && map->item_array[0].is_object());

		node_t* root_node=load_node(map->item_array[0].obj);

		delete map;
		return root_node;
	}
	else
	{
		return NULL;
	}
}