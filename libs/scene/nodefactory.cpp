#include "nodefactory.h"

	metaobject* metaobject_manager::m_metaobjectlist=NULL;
	int metaobject_manager::m_type_count=0;


	metaobject::metaobject(const char* i_typename, metaobject* i_parent):
	m_typename(i_typename),
	m_parent(i_parent),
	m_typeid(metaobject_manager::get_typeid())
	{
		metaobject_manager::add_metaobject(this);
	}
