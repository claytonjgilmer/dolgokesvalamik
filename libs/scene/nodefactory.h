#ifndef _nodefactory_h_
#define _nodefactory_h_

#include <stddef.h>

#include "containers/vector.h"
#include "containers/string.h"
#include "utils/assert.h"
#include "math/mtx4x3.h"
#include "scripting/lua.h"
#include "json/jsonparser.h"




	struct rootobject;

	enum proptype
	{
		prop_int,
		prop_float,
		prop_vec3,
		prop_mtx4x3,
		prop_string,
		prop_angle
	};

	struct property_descriptor
	{
		property_descriptor(const char* i_name,proptype i_type,unsigned i_offset):m_name(i_name),m_offset(i_offset),m_type(i_type)
		{
		}

		const char* get_name() const
		{
			return m_name;
		}

		proptype get_type() const
		{
			return m_type;
		}

		unsigned get_offset() const
		{
			return m_offset;
		}

		int get_int(rootobject* i_object) const
		{
			assertion(m_type==prop_int);

			return (int&)(*((char*)(i_object)+m_offset));
		}

		f32 get_float(rootobject* i_object) const
		{
			assertion(m_type==prop_float);

			return (f32&)(*((char*)(i_object)+m_offset));
		}

		vec3 get_vec3(rootobject* i_object) const
		{
			assertion(m_type==prop_vec3);

			return (vec3&)(*((char*)(i_object)+m_offset));
		}

		mtx4x3 get_mtx4x3(rootobject* i_object) const
		{
			assertion(m_type==prop_mtx4x3);

			return (mtx4x3&)(*((char*)(i_object)+m_offset));
		}

		string get_string(rootobject* i_object) const
		{
			assertion(m_type==prop_string);

			return (string&)(*((char*)(i_object)+m_offset));
		}

		float get_angle(rootobject* i_object) const
		{
			assertion(m_type==prop_angle);

			return radtodegree((f32&)(*((char*)(i_object)+m_offset)));
		}

		void load_value(rootobject* i_object, json_object_t* obj)
		{
			json_value_t* val=obj->get_value(m_name);

			if (val)
			{
				switch (m_type)
				{
				case prop_int:
					assertion(val->is_num());
					(int&)*((char*)i_object+m_offset)=(int)val->get_num();
					break;
				case prop_float:
					assertion(val->is_num());
					(float&)*((char*)i_object+m_offset)=(float)val->get_num();
					break;
				case prop_vec3:
					{
						assertion(val->is_array() && val->arr->value_list.size()==3);
						const json_array_t* arr=val->get_array();
						const vector<json_value_t>& vec=arr->value_list;
						((vec3&)*((char*)i_object+m_offset)).set((float)vec[0].get_num(),(float)vec[1].get_num(),(float)vec[2].get_num());
						break;
					}
				case prop_mtx4x3:
					{
						assertion(val->is_object() && val->obj->pair_list.size()==3);
						vec3 m[3];
						vector<json_pair_t>& p=val->obj->pair_list;

						for (unsigned n=0; n<3; ++n)
						{
							json_value_t* val=&p[n].val;
							assertion(val->is_array() && val->arr->value_list.size()==3);
							const json_array_t* arr=val->get_array();
							const vector<json_value_t>& vec=arr->value_list;
							m[n].set((float)vec[0].get_num(),(float)vec[1].get_num(),(float)vec[2].get_num());
						}

						mtx4x3 mtx;
						mtx.t=m[0];
						mtx.set_euler(degreetorad(m[1].x),degreetorad(m[1].y),degreetorad(m[1].z));
						mtx.x*=m[2].x;
						mtx.y*=m[2].y;
						mtx.z*=m[2].z;
						((mtx4x3&)*((char*)i_object+m_offset))=mtx;
						break;
					}
				case prop_string:
					assertion(val->is_str());
					((string&)*((char*)i_object+m_offset))=val->get_str();
					break;
				case prop_angle:
					assertion(val->is_num());
					(float&)*((char*)i_object+m_offset)=degreetorad((float)val->get_num());
					break;
				default:
					assertion(0);
				}
			}
		}

		void load_value(rootobject* i_object, lua::Variable& i_table)
		{
			lua::Variable var=i_table.GetVariable(m_name);

			switch (m_type)
			{
			case prop_int:
				{
					if (var.IsInt())
						(int&)*((char*)i_object+m_offset)=var.GetInt();
					break;
				}
			}

		}

		string to_string(rootobject* i_obj);

		const char* m_name;
		proptype m_type;
//		const char* m_type;
		unsigned m_offset;

	};//struct


	struct metaobject
	{
		metaobject(const char* i_typename, metaobject* i_parent);

		virtual rootobject* create() const=0;
		virtual rootobject* create(const rootobject& orig) const=0;

		metaobject* m_prev;
		metaobject* m_next;

		const char* get_typename() const
		{
			return m_typename;
		}

		int get_typeid() const
		{
			return m_typeid;
		}

		int isa(int i_typeid) const
		{
			const metaobject* ptr=this;

			while (ptr)
			{
				if (ptr->m_typeid==i_typeid)
					return true;

				ptr=ptr->m_parent;
			}

			return false;
		}

		unsigned get_propertycount() const
		{
			return m_proparray.size();
		}

		const property_descriptor* get_property(unsigned i_index) const
		{
			return &m_proparray[i_index];
		}

		void add_property(const char* i_name, proptype i_type, unsigned i_offset)
		{
			m_proparray.push_back(property_descriptor(i_name,i_type,i_offset));
		}

		void load_property(rootobject* i_object, lua::Variable& i_table)
		{
			for (unsigned n=0; n<m_proparray.size(); ++n)
				m_proparray[n].load_value(i_object,i_table);

			if (m_parent)
				m_parent->load_property(i_object,i_table);
		}

		void load_properties(rootobject* i_object, json_object_t* prop_obj)
		{
			for (unsigned n=0; n<m_proparray.size(); ++n)
				m_proparray[n].load_value(i_object,prop_obj);

			if (m_parent)
				m_parent->load_properties(i_object,prop_obj);
		}

		static string to_string(rootobject* i_object, proptype i_type,void* i_data)
		{
			switch (i_type)
			{
			case prop_string:
				return string(*(string*)i_data);
			case prop_mtx4x3:
			{
				mtx4x3 mtx((*(mtx4x3*)i_data));
				f32 rx,ry,rz;
				f32 sx,sy,sz;
				f32 tx,ty,tz;
				sx=mtx.x.length();
				sy=mtx.y.length();
				sz=mtx.z.length();


				mtx.normalize();
				mtx.get_euler(rx,ry,rz);

				tx=mtx.t.x;
				ty=mtx.t.y;
				tz=mtx.t.z;

				char str[1024];
				sprintf(str,"%.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f",rx,ry,rz,sx,sy,sz,tx,ty,tz);

				return str;
			}
			default:
				assertion(0);
			}

			return "";
		}

		metaobject* get_parent()
		{
			return m_parent;
		}

		struct property_iterator
		{
			metaobject* m_ptr;
			unsigned m_index;

			property_iterator():m_ptr(0),m_index(0){}
			property_iterator(metaobject* i_ptr,unsigned i_index):m_ptr(i_ptr),m_index(i_index){}

			int operator==(const property_iterator& i_other) const{return (m_ptr==i_other.m_ptr && m_index==i_other.m_index);}
			int operator!=(const property_iterator& i_other) const{return (m_ptr!=i_other.m_ptr || m_index!=i_other.m_index);}
			void operator++(){++m_index; while (m_ptr && m_index>=m_ptr->get_propertycount()){m_index=0; m_ptr=m_ptr->get_parent();}}
//			property_descriptor* operator->(){return m_ptr->get_property(m_index);}
			const property_descriptor* operator->() const {return m_ptr->get_property(m_index);}
//			property_descriptor& operator&(){return *m_ptr->get_property(m_index);}
			const property_descriptor& operator*() const {return *m_ptr->get_property(m_index);}
		};

		property_iterator begin()
		{
			property_iterator it;
			it.m_index=0;
			it.m_ptr=this;

			while (it.m_ptr && !it.m_ptr->get_propertycount())
				it.m_ptr=it.m_ptr->get_parent();

			return it;
		}

		property_iterator end()
		{
			return property_iterator(0,0);
		}

		const char* m_typename;
		int m_typeid;
		metaobject* m_parent;
		vector<property_descriptor> m_proparray;
	};

	MLINLINE string property_descriptor::to_string(rootobject* i_obj)
	{
		return metaobject::to_string(i_obj,m_type,(void*)(((char*)i_obj)+m_offset));
	}


//#define BIND_START(_object_) void _object_##_metaobject::bind_properties(){
//#define BIND_END }


	struct metaobject_manager
	{
	public:
		static void add_metaobject(metaobject* i_creator)
		{
			i_creator->m_next=m_metaobjectlist;
			if (m_metaobjectlist)
				m_metaobjectlist->m_prev=i_creator;
			i_creator->m_prev=0;
			m_metaobjectlist=i_creator;
		}

		static int get_typeid()
		{
			return m_type_count++;
		}

		static rootobject* create_object(const char* i_typename)
		{
			metaobject* ptr=get_metaobject(i_typename);

			if (ptr)
				return ptr->create();

			return NULL;
		}

		static metaobject* get_metaobject(const char* i_typename)
		{
			metaobject* ptr=m_metaobjectlist;

			while (ptr)
			{
				if (!strcmp(ptr->get_typename(),i_typename))
				{
					return ptr;
				}

				ptr=ptr->m_next;
			}

			assertion(0);
			return NULL;
		}

		static metaobject* m_metaobjectlist;
		static int m_type_count;
	};//struct

template<typename T> struct prop_binder
{
	prop_binder(const char* i_name, proptype i_type, unsigned i_offset)
	{
		T::get_class_metaobject()->add_property(i_name,i_type,i_offset);
	}
};
//#define VAR_NAME(_neve_,_hol_,_hol2_) _neve_##_hol_##_hol2_
#define BIND_PROPERTY_NAME(_object_,_variable_,_name_,_type_) prop_binder<_object_> g_##_object_##_variable_(_name_,prop_##_type_,(unsigned)offsetof(_object_,_variable_))
#define BIND_PROPERTY(_object_,_variable_,_type_) prop_binder<_object_> g_##_object_##_variable_(#_variable_,prop_##_type_,(unsigned)offsetof(_object_,_variable_))


#endif//_nodefactory_h_
