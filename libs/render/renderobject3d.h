#ifndef _renderobject3d_h_
#define _renderobject3d_h_

#include "scene/node.h"
#include "render/mesh.h"
#include "utils/auto_ptr.h"
#include "containers/fixedvector.h"

	struct object3d:public node_t
	{
		DECLARE_OBJECT(object3d);
//	public:
		object3d();
		object3d(const char* i_name);
		~object3d();
		void add_mesh(mesh_t* i_mesh){m_mesh.push_back(i_mesh);}
		mesh_t* get_mesh(unsigned i_index){return &*m_mesh[i_index];}
		unsigned get_meshnum() const{return m_mesh.size();}

		void render();

		virtual void on_load();

		object3d* Next;

		//properties
		string m_modelname;
//	protected:
		fixedvector<ref_ptr<mesh_t>,8> m_mesh;
	};
#endif//_renderobject3d_h_
