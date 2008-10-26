#ifndef _rootobject_h_
#define _rootobject_h_

#include "containers/string.h"
#include "nodefactory.h"

	class metaobject;

	class rootobject
	{
	public:
		rootobject(){}
		rootobject(const char* i_name):m_name(i_name){}
		virtual int get_typeid() const;
		virtual metaobject* get_metaobject() const;
		static metaobject* get_class_metaobject();

		const string& get_name() const;
		void set_name(const char* i_name);

		string m_name;
	};

#define DECLARE_OBJECT(_object_) \
	public:\
	friend class _object_##_metaobject;\
	friend class prop_binder<_object_>;\
	virtual int get_typeid() const;\
	virtual metaobject* get_metaobject() const;\
	static metaobject* get_class_metaobject();

#define  DEFINE_OBJECT(_object_,_parent_)\
	class _object_##_metaobject:public metaobject\
	{\
	public:\
	_object_##_metaobject():metaobject(#_object_,_parent_::get_class_metaobject()){}\
	rootobject* create() const{return new _object_;}\
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
	metaobject* _object_::get_metaobject() const\
	{\
		return &g_##_object_##_metaobject;\
	}\
\
	metaobject* _object_::get_class_metaobject()\
	{\
		return &g_##_object_##_metaobject;\
	}
#endif//_rootobject_h_