#ifndef _node_h_
#define _node_h_

#include "containers/string.h"
#include "base/assert.h"

namespace scene
{
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

	protected:
		node* m_parent;
		node* m_child;
		node* m_bro;

		ctr::string m_name;
	};
}
#endif//_node_h_