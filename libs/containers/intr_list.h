#ifndef _INTR_LIST_H_
#define _INTR_LIST_H_

//template<typename basetype>



template <typename intr_list_node>
struct intr_list_node_base
{
    intr_list_node* prev;
    intr_list_node* next;
//    basetype* elem;
};


//template<typename basetype>
template <typename intr_list_node>
struct intr_list
{
    intr_list_node _head;
    intr_list_node  _end;

    intr_list()
    {
        _head.prev=0;
        _head.next=&_end;

        _end.next=0;
        _end.prev=&_head;
    }

    void push_back(intr_list_node* newnode)
    {
        newnode->prev=this->_end.prev;
        newnode->next=&this->_end;
        this->_end.prev->next=newnode;
        this->_end.prev=newnode;
    }

    void push_front(intr_list_node* newnode)
    {
        newnode->prev=&this->_head;
        newnode->next=this->_head.next;
        this->_head.next->prev=newnode;
        this->_head.next=newnode;
    }

    void erase(intr_list_node* newnode)
    {
        newnode->prev->next=newnode->next;
        newnode->next->prev=newnode->prev;
    }

/*
    struct iterator
    {
        iterator(){n=0;}
        iterator(intr_list_node* n){this->n=n;}
        intr_list_node* operator*()
        {
            return n;
        }

        void operator++()
        {
            n=n->next;
        }

        int operator !=(const iterator& other) const
        {
            return n!=other.n;
        }

        intr_list_node* n;
    };
*/
    intr_list_node* first()
    {
        return _head.next;
    }

    intr_list_node* last()
    {
        return &_end;
    }
/*
    iterator begin()
    {
        return _head.next;
    }

    iterator end()
    {
        return &_end;
    }
*/
};

#endif // _INTR_LIST_H_
