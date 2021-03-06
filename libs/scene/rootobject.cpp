#include "rootobject.h"
#include "nodefactory.h"


	struct rootobject_metaobject:metaobject
	{
		rootobject_metaobject():metaobject("rootobject",NULL){}
		rootobject* create() const{return new rootobject;}
		rootobject* create(const rootobject& orig) const {return new rootobject(orig);}
	};

	rootobject_metaobject g_rootobject_metaobject;
	BIND_PROPERTY(rootobject,name,string);





	int rootobject::get_typeid() const
	{
		return g_rootobject_metaobject.get_typeid();
	}

	metaobject* rootobject::get_metaobject() const
	{
		return &g_rootobject_metaobject;
	}

	metaobject* rootobject::get_class_metaobject()
	{
		return &g_rootobject_metaobject;
	}


//	DEFINE_OBJECT(rootobject,NULL);

	const string& rootobject::get_name() const
	{
		return name;
	}

	void rootobject::set_name(const char* i_name)
	{
		name=i_name;
	}
