#include "renderobject3d.h"
#include "rendersystem.h"
#include "objectmanager.h"
//#include "scene/nodefactory.h"

	DEFINE_OBJECT(object3d,node_t);
	BIND_PROPERTY(object3d,m_modelname,"modelname",string);

	object3d::object3d():
	color(1,1,1,1)
	{
		m_flags|=nodeflag_dont_save;
		param_array.bind_param("model_color",&color,4*sizeof(float));
	}

	node_t* object3d::clone()
	{
		object3d* obj=(object3d*)node_t::clone();
		obj->param_array.param_count=0;
		obj->param_array.bind_param("model_color",&obj->color,4*sizeof(float));

		return obj;
	}

	object3d::object3d(const char* i_name):
	node_t(i_name),
	color(1,1,1,1)
	{
		param_array.bind_param("model_color",&color,4*sizeof(float));
	}

	object3d::~object3d()
	{
	}

	void object3d::render()
	{
		object3d* ptr=this;

		while (ptr)
		{
			for (unsigned n=0; n<ptr->m_mesh.size(); ++n)
				rendersystem::ptr->add_renderable(ptr->m_mesh[n].get(),&param_array,ptr->get_worldposition());

			ptr=(object3d*)ptr->get_next();
		}
	}

	void object3d::on_load()
	{
		if (m_modelname!="")
		{
			object3d* obj=objectmanager::ptr->get_object(m_modelname.c_str());
			if (obj)
				add_child(obj);
		}
	}
