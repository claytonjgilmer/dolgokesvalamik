#include "node.h"
#include "containers/vector.h"

namespace scene
{
DEFINE_OBJECT(node,rootobject);
BIND_PROPERTY(node,get_localposition(),"localpos",mtx4x3);

	node::node(const char* i_name):
	rootobject(i_name),
	m_localpos(math::mtx4x3::identitymtx()),
	m_worldpos(math::mtx4x3::identitymtx())
	{
		m_parent=NULL;
		m_child=NULL;
		m_bro=NULL;
		m_flags|=nodeflag_valid_worldpos;
	}


	node::node():
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
		if (m_parent)
			m_parent->remove_child(this);

		ctr::vector<node*> subh;



		node* ptr=m_child;

		while (ptr)
		{
			subh.push_back(ptr);
			ptr=ptr->get_next(this);
		}

		for (unsigned n=0; n<subh.size(); ++n)
		{
			subh[n]->m_child=0;
			subh[n]->m_parent=0;
			subh[n]->m_bro=0;

			delete subh[n];
		}
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
		utils::assertion(i_child->get_parent()==this,"nonono!");


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
	
	node* node::get_next(node* i_root) const
	{
		if (m_child)
			return m_child;
			
		const node* ptr=this;
		
		while (ptr!=i_root)
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
		
		node* ptr=this;
		
		while (ptr)
		{
			ptr->m_flags&=~nodeflag_valid_worldpos;
			ptr=ptr->get_next(this);
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
		if (m_parent)
			parentinvworld.invert(m_parent->get_worldposition());
		else
			parentinvworld.identity();

		m_localpos.multiply(m_worldpos,parentinvworld);

		m_flags|=nodeflag_valid_worldpos;

		node* ptr=this;
		ptr=ptr->get_next(this);
		
		while (ptr)
		{
			ptr->m_flags&=~nodeflag_valid_worldpos;
			ptr=ptr->get_next(this);
		}
	}

	const math::mtx4x3& node::get_worldposition()
	{
		node* buf[128];
		int bufsize=0;
		node* ptr=this;

		while (ptr->m_parent && !(ptr->m_flags & nodeflag_valid_worldpos))
		{
			buf[bufsize++]=ptr;
			ptr=ptr->m_parent;
		}

		for (int n=bufsize-1; n>=0; --n)
		{
			buf[n]->m_worldpos.multiply(buf[n]->m_localpos,buf[n]->m_parent->m_worldpos);
			buf[n]->m_flags|=nodeflag_valid_worldpos;
		}

		return m_worldpos;
	}

	void node::set_flag(unsigned i_flag)
	{
		m_flags|=i_flag;
	}

	void node::clr_flag(unsigned i_flag)
	{
		m_flags&=~i_flag;
	}

	node* node::get_node_by_name(const char* i_name)
	{
		for (node* ptr=this; ptr; ptr=ptr->get_next())
		{
			if (ptr->m_name==i_name)
				return ptr;
		}

		return NULL;
	}

}