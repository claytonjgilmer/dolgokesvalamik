#ifndef _nodefactory_h_
#define _nodefactory_h_

#include "containers/vector.h"
#include "utils/assert.h"




namespace scene
{
	class property_descriptor
	{
	public:
		property_descriptor(const char* i_name,const char* i_type,unsigned i_offset):m_name(i_name),m_offset(i_offset),m_type(i_type)
		{
		}

		const char* get_name() const
		{
			return m_name;
		}

		const char* get_type() const
		{
			return m_type;
		}

		unsigned get_offset() const
		{
			return m_offset;
		}

	private:
		const char* m_name;
		const char* m_type;
		unsigned m_offset;

	};//class

	class  rootobject;

	class metaobject
	{
	public:
		metaobject(const char* i_typename, metaobject* i_parent);

		virtual rootobject* create() const=0;

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

		bool isa(int i_typeid) const
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

	protected:
		const char* m_typename;
		int m_typeid;
		metaobject* m_parent;
		ctr::vector<property_descriptor> m_proparray;
	};

	class metaobject_manager
	{
	public:
		static void add_metaobject(metaobject* i_creator)
		{
			i_creator->m_next=m_metaobjectlist;
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
			}

			utils::assertion(0);
			return NULL;
		}

		static metaobject* m_metaobjectlist;
		static int m_type_count;
	};//class
}//namespace
#endif//_nodefactory_h_
