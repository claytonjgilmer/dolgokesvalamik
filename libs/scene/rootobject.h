#ifndef _rootobject_h_
#define _rootobject_h_

#include "containers/string.h"
#include "nodefactory.h"

namespace scene
{
	class metaobject;

	class rootobject
	{
	public:
		rootobject(){}
		rootobject(const char* i_name):m_name(i_name){}
		virtual int get_typeid() const;
		virtual metaobject* get_metaobject() const;
		static metaobject* get_class_metaobject();

		const ctr::string& get_name() const;
		void set_name(const char* i_name);

		ctr::string m_name;
	};

#define DECLARE_OBJECT(_object_) \
	public:\
	friend class _object_##_metaobject;\
	friend class prop_binder<_object_>;\
	virtual int get_typeid() const;\
	virtual scene::metaobject* get_metaobject() const;\
	static scene::metaobject* get_class_metaobject();

#define  DEFINE_OBJECT(_object_,_parent_)\
	class _object_##_metaobject:public scene::metaobject\
	{\
	public:\
	_object_##_metaobject():scene::metaobject(#_object_,_parent_::get_class_metaobject()){}\
	scene::rootobject* create() const{return new _object_;}\
	void bind_properties();\
	};\
\
	_object_##_metaobject g_##_object_##_metaobject;\
\
	int _object_::get_typeid() const\
	{\
		return g_##_object_##_metaobject.get_typeid();\
	}\
\
	scene::metaobject* _object_::get_metaobject() const\
	{\
		return &g_##_object_##_metaobject;\
	}\
\
	scene::metaobject* _object_::get_class_metaobject()\
	{\
		return &g_##_object_##_metaobject;\
	}



}//namespace
#endif//_rootobject_h_