#ifndef _renderobject3d_h_
#define _renderobject3d_h_

#include "scene/node.h"
#include "render/mesh.h"
#include "utils/auto_ptr.h"
#include "utils/color.h"
#include "containers/fixedvector.h"
#include "shaderparam.h"

	struct object3d : node_t
	{
		DECLARE_OBJECT(object3d);

		object3d();
		object3d(const char* i_name);
		~object3d();
		void add_mesh(mesh_t* i_mesh){m_mesh.push_back(i_mesh);}
		mesh_t* get_mesh(unsigned i_index){return &*m_mesh[i_index];}
		unsigned get_meshnum() const{return m_mesh.size();}
		node_t* clone();

		void render();

		virtual void on_load();

		object3d* Next;

		//properties
		string m_modelname;
		fixedvector<ref_ptr<mesh_t>,8> m_mesh;
		color_f color;
		shader_param_array_t param_array;
	};
#endif//_renderobject3d_h_
