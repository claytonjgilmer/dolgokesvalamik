#ifndef _list_allocator_h_
#define _list_allocator_h_

#include "vector.h"

	template <typename basetype, int blocksize=128> struct list_allocator //blocksize kettohatvany!
	{
		struct elem_t
		{
			char m_data[sizeof(basetype)];
			elem_t* m_prev;
			elem_t* m_next;
		};

		struct iterator
		{
			iterator()
			{
				m_elem=NULL;
			}

			iterator(basetype* i_elem)
			{
				m_elem=(elem_t*)i_elem;
			}

			iterator(elem_t* i_elem)
			{
				m_elem=i_elem;
			}

			basetype* operator *() const
			{
				return (basetype*)(&m_elem->m_data);
			}

			void operator++()
			{
				m_elem=m_elem->m_next;
			}

			void operator--()
			{
				m_elem=m_elem->m_prev;
			}

			int operator==(const iterator& i_other) const
			{
				return m_elem==i_other.m_elem;
			}

			int operator!=(const iterator& i_other) const
			{
				return m_elem!=i_other.m_elem;
			}

			elem_t* m_elem;
		};

		list_allocator();
		~list_allocator();
		basetype* allocate();
		void deallocate(basetype*);
		void deallocate_place(basetype*);
		void deallocate_all();
		basetype* allocate_place();
		iterator begin()
		{
			return iterator(m_head.m_next);
		}

		iterator end()
		{
			return iterator(&m_end);
		}

		iterator head()
		{
			return iterator(&m_head);
		}

		void moveafter(iterator i_elem, iterator i_after);
		void movebefore(iterator i_elem, iterator i_before);
		unsigned size() const;

		vector<elem_t*> m_allocated;
		elem_t* m_free;
		elem_t  m_head;
		elem_t  m_end;
		unsigned m_size;

		void allocfree();
	}; //struct  list_allocator

	template <typename basetype,int blocksize>
	MLINLINE void list_allocator<basetype,blocksize>::allocfree()
	{
		elem_t* newelem=(elem_t*)malloc(blocksize*sizeof(elem_t));

		for (int n=0;n<blocksize-1; ++n)
			newelem[n].m_next=&newelem[n+1];

		newelem[blocksize-1].m_next=NULL;
		m_free=newelem;
		m_allocated.push_back(newelem);
	}

	template <typename basetype,int blocksize>
	MLINLINE basetype* list_allocator<basetype,blocksize>::allocate()
	{
		elem_t* newelem;
		if (!m_free)
			allocfree();
		newelem=m_free;

		new(&(newelem->m_data)) basetype();
		m_free=m_free->m_next;
		m_size++;

		newelem->m_next=&m_end;
		newelem->m_prev=m_end.m_prev;
		m_end.m_prev->m_next=newelem;
		m_end.m_prev=newelem;

		return (basetype*)(&newelem->m_data);
	}

	template <typename basetype,int blocksize>
	MLINLINE basetype* list_allocator<basetype,blocksize>::allocate_place()
	{
		elem_t* newelem;

		if (!m_free)
			allocfree();

		newelem=m_free;

		m_free=m_free->m_next;
		m_size++;

		newelem->m_next=&m_end;
		newelem->m_prev=m_end.m_prev;
		m_end.m_prev->m_next=newelem;
		m_end.m_prev=newelem;

		return (basetype*)(&newelem->m_data);
	}

	template <typename basetype, int blocksize>
	MLINLINE void list_allocator<basetype,blocksize>::deallocate(basetype* i_data)
	{
		i_data->~basetype();
		deallocate_place(i_data);
	}

	template <typename basetype, int blocksize>
	MLINLINE void list_allocator<basetype,blocksize>::deallocate_place(basetype* i_data)
	{
		elem_t* actelem=(elem_t*)i_data;

		actelem->m_next->m_prev=actelem->m_prev;
		actelem->m_prev->m_next=actelem->m_next;

		actelem->m_next=m_free;

		m_free=actelem;
		m_size--;
	}

	template <typename basetype, int blocksize>
	MLINLINE void list_allocator<basetype,blocksize>::deallocate_all()
	{
		if (!m_size)
			return;

		elem_t* ptr=m_head.m_next;

		while (ptr->m_next!=&m_end)
		{
			((basetype*)ptr)->~basetype();
			ptr=ptr->m_next;
		}

		((basetype*)ptr)->~basetype();

		ptr->m_next=m_free;
		m_free=m_head.m_next;

		m_head.m_next=&m_end;
		m_end.m_prev=&m_head;
		m_size=0;
	}

	template <typename basetype,int blocksize>
	list_allocator<basetype,blocksize>::list_allocator()
	{
		m_end.m_prev=&m_head;
		m_end.m_next=NULL;
		m_head.m_prev=NULL;
		m_head.m_next=&m_end;
		m_free=NULL;
		m_size=0;
		allocfree();
	}

	template <typename basetype, int blocksize>
	MLINLINE list_allocator<basetype,blocksize>::~list_allocator()
	{
		deallocate_all();
		for (unsigned int n=0; n<m_allocated.size(); ++n)
			free(m_allocated[n]);
	}

	template <typename basetype, int blocksize>
	MLINLINE unsigned list_allocator<basetype,blocksize>::size() const
	{
		return m_size;
	}

	template<typename basetype, int blocksize>
	MLINLINE void list_allocator<basetype,blocksize>::moveafter(iterator i_elem, iterator i_after)
	{
		elem_t* e=i_elem.m_elem;
		elem_t* after=i_after.m_elem;
		dynassert(i_after!=i_elem);
		if (after->m_next==e)
			return;

		//kifuz
		e->m_prev->m_next=e->m_next;
		e->m_next->m_prev=e->m_prev;

		//befuz
		e->m_next=after->m_next;
		e->m_prev=after;

		after->m_next->m_prev=e;
		after->m_next=e;
	}

	template<typename basetype, int blocksize>
	MLINLINE void list_allocator<basetype,blocksize>::movebefore(iterator i_elem, iterator i_before)
	{
		elem_t* e=i_elem.m_elem;
		elem_t* before=i_before.m_elem;
		dynassert(i_elem!=i_before);
		if (before->m_prev==e)
			return;


		//kifuz
		e->m_prev->m_next=e->m_next;
		e->m_next->m_prev=e->m_prev;

		//befuz
		e->m_next=before;
		e->m_prev=before->m_prev;
		before->m_prev->m_next=e;
		before->m_prev=e;
	}
#endif//_list_allocator_h_
