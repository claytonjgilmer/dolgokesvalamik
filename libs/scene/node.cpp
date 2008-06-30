#include "node.h"

namespace scene
{
	node::node()
	{
		m_parent=NULL;
		m_child=NULL;
		m_bro=NULL;
	}

	node::~node()
	{
	}

	node* node::get_bro() const
	{
		return m_bro;
	}

	node* node::get_child() const
	{
		return m_child;
	}

	node* node::get_parent() const
	{
		return m_parent;
	}

	void node::add_child(node* i_child)
	{
		if (i_child->get_parent())
			i_child->get_parent()->remove_child(i_child);

		i_child->m_parent=this;
		i_child->m_bro=m_child;
		m_child=i_child;
	}

	void node::remove_child(node* i_child)
	{
		base::assertion(i_child->get_parent()==this,"nonono!");


		node* ptr=m_child;
		node* prevptr=NULL;

		while (ptr!=i_child)
		{
			prevptr=ptr;
			ptr=ptr->m_bro;
		}

		if (prevptr)
			prevptr->m_bro=i_child->m_bro;
		else
			m_child=i_child->m_bro;

		i_child->m_parent=NULL;
		i_child->m_bro=NULL;
	}
}