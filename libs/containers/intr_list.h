#ifndef _INTR_LIST_H_
#define _INTR_LIST_H_

//template<typename basetype>
#include "utils/misc.h"



//////////////////////////////////////////////////////////////////////////
//		intrusive list
//////////////////////////////////////////////////////////////////////////
template <typename intr_list_node>
struct intr_list_node_base
{
	intr_list_node* next;
    intr_list_node* prev;
};


template <typename intr_list_node>
struct intr_list
{
    char _head_buf[sizeof(intr_list_node)];
    char _end_buf[sizeof(intr_list_node)];

#ifdef _DEBUG
	intr_list_node* head_ptr;
	intr_list_node* end_ptr;
#endif//_DEBUG

#define _intr_list_head (*(intr_list_node*)_head_buf)
#define _intr_list_end (*(intr_list_node*)_end_buf)

    intr_list()
    {
        _intr_list_head.prev=0;
        _intr_list_head.next=&_intr_list_end;

        _intr_list_end.next=0;
        _intr_list_end.prev=&_intr_list_head;

#ifdef _DEBUG
		head_ptr=(intr_list_node*)_head_buf;
		end_ptr=(intr_list_node*)_end_buf;
#endif//_DEBUG
    }

    void push_back(intr_list_node* newnode)
    {
        newnode->prev=_intr_list_end.prev;
        newnode->next=&_intr_list_end;
        _intr_list_end.prev->next=newnode;
        _intr_list_end.prev=newnode;
    }

    void push_front(intr_list_node* newnode)
    {
        newnode->prev=&_intr_list_head;
        newnode->next=_intr_list_head.next;
        _intr_list_head.next->prev=newnode;
        _intr_list_head.next=newnode;
    }

    void erase(intr_list_node* newnode)
    {
        newnode->prev->next=newnode->next;
        newnode->next->prev=newnode->prev;
    }

    intr_list_node* first()
    {
        return _intr_list_head.next;
    }

    intr_list_node* last()
    {
        return &_intr_list_end;
    }
};


//////////////////////////////////////////////////////////////////////////
//		kisebb helyigenyu intrusive lista
//////////////////////////////////////////////////////////////////////////
template <typename intr_list_node>
struct intr_list_simple
{
	intr_list_node* first_elem;
	intr_list_simple()
	{
		first_elem=NULL;
	}

	void push_front(intr_list_node* newnode)
	{
		newnode->next=first_elem;
		newnode->prev=NULL;
		if (first_elem)
			first_elem->prev=newnode;

		first_elem=newnode;
	}

	void erase(intr_list_node* node)
	{
		if (node->prev)
			node->prev->next=node->next;
		else
			first_elem=node->next;

		if (node->next)
			node->next->prev=node->prev;
	}

	intr_list_node* first()
	{
		return first_elem;
	}

};






//////////////////////////////////////////////////////////////////////////
//		circular list
//////////////////////////////////////////////////////////////////////////
template <typename intr_list_node>
struct intr_circular_list
{
	intr_list_node* first_elem;

	intr_circular_list()
	{
		first_elem=NULL;
	}

	void push_back(intr_list_node* newnode) //first_elem ele, first_elem nem valtozik (ha mar van first_elem)
	{
		if (!first_elem)
		{
			newnode->next=newnode;
			newnode->prev=newnode;
			first_elem=newnode;
		}
		else
		{
			insert_before(newnode,first_elem);
		}
	}

	void push_front(intr_list_node* newnode)
	{
		if (!first_elem)
		{
			newnode->next=newnode;
			newnode->prev=newnode;
			first_elem=newnode;
		}
		else
		{
			insert_before(first_elem);
		}

		first_elem=newnode;
	}

	void insert_before(intr_list_node* newnode, intr_list_node* before)
	{
		newnode->next=before;
		newnode->prev=before->prev;
		before->prev->next=newnode;
		before->prev=newnode;
	}

	void erase(intr_list_node* node)
	{
		if (node->next==node)
			first_elem=NULL;
		else
		{
			node->prev->next=node->next;
			node->next->prev=node->prev;
			if (first_elem==node)
				first_elem=first_elem->next;
		}
	}

	void clear()
	{
		first_elem=NULL;
	}

	intr_list_node* first()
	{
		return first_elem;
	}

	intr_list_node* last()
	{
		return first_elem->prev;
	}
};

#endif // _INTR_LIST_H_
