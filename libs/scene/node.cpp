#include "node.h"

namespace scene
{
	node::node()
	m_localpos(math::mtx4x3::identitymtx()),
	m_worldpos(math::mtx4x3::identitymtx())
	{
		m_parent=NULL;
		m_child=NULL;
		m_bro=NULL;
		m_flags|=nodeflag_valid_worldpos;
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
	
	node* node::get_next() const
	{
		if (m_child)
			return m_child;
			
		node* ptr=this;
		
		while (ptr)
		{
			if (ptr->m_bro)
				return ptr->m_bro;
				
			ptr=ptr->m_parent;
		}
		
		return NULL;
	}
	
	void node::set_localposition(const math::mtx4x3& i_mtx)
	{
		m_localpos=i_mtx;
		
		//ennek a node-nak es a childnode-oknak ilyenkor invalidalodik a worldmatrixa
		m_flags&=~nodeflag_valid_worldpos;
		
		node* ptr=m_child;
		
		if (ptr)
		{
			while (ptr!=this)
			{
				ptr->m_flags&=~nodeflag_valid_worldpos;
				ptr=ptr->get_next();
			}
		}
	}
	const math::mtx4x3& node::get_localposition() const
	{
		//localpos mindig valid;
		return m_localpos;
	}
	
	void node::set_worldposition(const math::mtx4x3& i_mtx)
	{
		m_worldpos=i_mtx;
		math::mtx4x3 parentinvworld;
		parentinvworld.invert(m_parent->get_worldposition());
		m_localpos.multiply(m_worldpos,parentinvworld);
		
		m_flags|=nodeflag_valid_worldpos;
		
		//
		node* ptr=m_child;
		
		if (ptr)
		{
			while (ptr!=this)
			{
				ptr->m_flags&=~nodeflag_valid_worldpos;
				ptr=ptr->get_next();
			}
		}
	}
	
	const math::mtx4x3& get_worldposition() const;
	
}