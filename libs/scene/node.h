#ifndef _node_h_
#define _node_h_

#include "rootobject.h"
#include "containers/string.h"
#include "utils/assert.h"
#include "math/mtx4x3.h"
#include "prop/property.h"

	const unsigned nodeflag_valid_worldpos=(1<<0);
	const unsigned nodeflag_dont_save=(1<<0);
	
	class node:public rootobject
	{
		DECLARE_OBJECT(node);
	public:
		node();
		node(const char* i_name);
		virtual ~node();

		node* get_parent() const;
		node* get_child() const;
		node* get_bro() const;

		void add_child(node* i_child);
		void remove_child(node* i_child);

		void set_localposition(const mtx4x3&);
		const mtx4x3& get_localposition() const;
		
		void set_worldposition(const mtx4x3&);
		const mtx4x3& get_worldposition();
		
		node* get_next(node* i_root=NULL) const;

		node* get_node_by_name(const char* i_name);

		void set_flag(unsigned i_flag);
		void clr_flag(unsigned i_flag);

		virtual void on_load(){}

		
	protected:
		node* m_parent;
		node* m_child;
		node* m_bro;

//		string m_name;
		mtx4x3 m_localpos;
		mtx4x3 m_worldpos;
		
		unsigned m_flags;
	};
#endif//_node_h_
