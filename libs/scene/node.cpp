#include "node.h"
#include "containers/vector.h"

DEFINE_OBJECT(node_t,rootobject);
BIND_PROPERTY(node_t,m_localpos,"localpos",mtx4x3);

	node_t::node_t(const char* i_name):
	rootobject(i_name),
	m_localpos(mtx4x3::identitymtx()),
	m_worldpos(mtx4x3::identitymtx())
	{
		m_parent=NULL;
		m_child=NULL;
		m_bro=NULL;
		m_flags|=nodeflag_valid_worldpos;
	}


	node_t::node_t():
	m_localpos(mtx4x3::identitymtx()),
	m_worldpos(mtx4x3::identitymtx())
	{
		m_parent=NULL;
		m_child=NULL;
		m_bro=NULL;
		m_flags|=nodeflag_valid_worldpos;
	}

	node_t::~node_t()
	{
		if (m_parent)
			m_parent->remove_child(this);

		vector<node_t*> subh;



		node_t* ptr=m_child;

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

	node_t* node_t::get_bro() const
	{
		return m_bro;
	}

	node_t* node_t::get_child() const
	{
		return m_child;
	}

	node_t* node_t::get_parent() const
	{
		return m_parent;
	}

	void node_t::add_child(node_t* i_child)
	{
		if (i_child->get_parent())
			i_child->get_parent()->remove_child(i_child);

		i_child->m_parent=this;
		i_child->m_bro=m_child;
		m_child=i_child;
	}

	void node_t::remove_child(node_t* i_child)
	{
		assertion(i_child->get_parent()==this,"nonono!");


		node_t* ptr=m_child;
		node_t* prevptr=NULL;

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

	node_t* node_t::get_next(node_t* i_root) const
	{
		if (m_child)
			return m_child;

		const node_t* ptr=this;

		while (ptr!=i_root)
		{
			if (ptr->m_bro)
				return ptr->m_bro;

			ptr=ptr->m_parent;
		}

		return NULL;
	}

	void node_t::set_localposition(const mtx4x3& i_mtx)
	{
		m_localpos=i_mtx;

		//ennek a node-nak es a childnode-oknak ilyenkor invalidalodik a worldmatrixa
		m_flags&=~nodeflag_valid_worldpos;

		node_t* ptr=this;

		while (ptr)
		{
			ptr->m_flags&=~nodeflag_valid_worldpos;
			ptr=ptr->get_next(this);
		}
	}
	const mtx4x3& node_t::get_localposition() const
	{
		//localpos mindig valid;
		return m_localpos;
	}

	void node_t::set_worldposition(const mtx4x3& i_mtx)
	{
		m_worldpos=i_mtx;

		mtx4x3 parentinvworld;
		if (m_parent)
			parentinvworld.invert(m_parent->get_worldposition());
		else
			parentinvworld.identity();

		m_localpos.multiply(m_worldpos,parentinvworld);

		m_flags|=nodeflag_valid_worldpos;

		node_t* ptr=this;
		ptr=ptr->get_next(this);

		while (ptr)
		{
			ptr->m_flags&=~nodeflag_valid_worldpos;
			ptr=ptr->get_next(this);
		}
	}

	const mtx4x3& node_t::get_worldposition()
	{
		node_t* buf[128];
		int bufsize=0;
		node_t* ptr=this;

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

	void node_t::set_flag(unsigned i_flag)
	{
		m_flags|=i_flag;
	}

	void node_t::clr_flag(unsigned i_flag)
	{
		m_flags&=~i_flag;
	}

	node_t* node_t::get_node_by_name(const char* i_name)
	{
		for (node_t* ptr=this; ptr; ptr=ptr->get_next())
		{
			if (ptr->m_name==i_name)
				return ptr;
		}

		return NULL;
	}
