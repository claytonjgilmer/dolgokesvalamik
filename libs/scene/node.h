#ifndef _node_h_
#define _node_h_

#include "rootobject.h"
#include "containers/string.h"
#include "utils/assert.h"
#include "math/mtx4x3.h"
#include "prop/property.h"

	const unsigned nodeflag_valid_worldpos=(1<<0);
	const unsigned nodeflag_dont_save=(1<<0);

	struct node_t:public rootobject
	{
		DECLARE_OBJECT(node_t);
	public:
		node_t();
		node_t(const char* i_name);
		virtual ~node_t();

		node_t* get_parent() const;
		node_t* get_child() const;
		node_t* get_bro() const;

		void add_child(node_t* i_child);
		void remove_child(node_t* i_child);

		void set_localposition(const mtx4x3&);
		const mtx4x3& get_localposition() const;

		void set_worldposition(const mtx4x3&);
		const mtx4x3& get_worldposition();

		node_t* get_next(node_t* i_root=NULL) const;

		node_t* get_node_by_name(const char* i_name);

		void set_flag(unsigned i_flag);
		void clr_flag(unsigned i_flag);

		virtual void on_load(){}

		virtual node_t* clone();

		virtual void init(){}
		virtual void exit(){}
		virtual void execute(){}
		virtual void render(){}


//	protected:
		node_t* m_parent;
		node_t* m_child;
		node_t* m_bro;

//		string m_name;
		mtx4x3 m_localpos;
		mtx4x3 m_worldpos;

		unsigned m_flags;
	};
#endif//_node_h_
