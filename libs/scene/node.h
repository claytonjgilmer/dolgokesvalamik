#ifndef _node_h_
#define _node_h_

#include "containers/string.h"
#include "utils/assert.h"
#include "math/mtx4x3.h"

namespace scene
{
	const unsigned nodeflag_valid_worldpos=(1<<0);
	
	class node
	{
	public:
		node();
		virtual ~node();

		node* get_parent() const;
		node* get_child() const;
		node* get_bro() const;

		void add_child(node* i_child);
		void remove_child(node* i_child);

		void set_localposition(const math::mtx4x3&);
		const math::mtx4x3& get_localposition() const;
		
		void set_worldposition(const math::mtx4x3&);
		const math::mtx4x3& get_worldposition();
		
		node* get_next(node* i_root=NULL) const;

		const ctr::string& get_name() const;
		
	protected:
		node* m_parent;
		node* m_child;
		node* m_bro;

		ctr::string m_name;
		math::mtx4x3 m_localpos;
		math::mtx4x3 m_worldpos;
		
		unsigned m_flags;
	};
}
#endif//_node_h_