#ifndef _INTR_LIST_H_
#define _INTR_LIST_H_

template<typename basetype>
struct intr_list_node
{
    intr_list_node* prev;
    intr_list_node* next;
    basetype* elem;
};


template<typename basetype>
struct intr_list
{
    intr_list_node<basetype> _head;
    intr_list_node<basetype> _end;

    intr_list()
    {
        _head.prev=0;
        _head.next=&_end;

        _end.next=0;
        _end.prev=&_head;

        _head.elem=_end.elem=0;
    }

    void push_back(intr_list_node<basetype>* newnode)
    {
        newnode->prev=this->_end.prev;
        newnode->next=&this->_end;
        this->_end.prev->next=newnode;
        this->_end.prev=newnode;
    }

    void push_front(intr_list_node<basetype>* newnode)
    {
        newnode->prev=&this->_head;
        newnode->next=this->_head.next;
        this->_head.next->prev=newnode;
        this->_head.next=newnode;
    }

    void erase(intr_list_node<basetype>* newnode)
    {
        newnode->prev->next=newnode->next;
        newnode->next->prev=newnode->prev;
    }

    struct iterator
    {
        iterator(){n=0;}
        iterator(intr_list_node<basetype>* n){this->n=n;}
        basetype* operator*()
        {
            return n->elem;
        }

        void operator++()
        {
            n=n->next;
        }

        int operator !=(const iterator& other) const
        {
            return n!=other.n;
        }

        intr_list_node<basetype>* n;
    };

    iterator begin()
    {
        return _head.next;
//        iterator it; it.n=_head.next; return it;
    }

    iterator end()
    {
        return &_end;
//        iterator it; it.n=&_end; return it;
    }
};

#endif // _INTR_LIST_H_
