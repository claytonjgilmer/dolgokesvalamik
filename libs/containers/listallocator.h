#ifndef _listallocator_h_
#define _listallocator_h_

#include "vector.h"

namespace ctr
{

	template <class basetype, int blocksize=128> class listallocator //blocksize kettohatvany!
	{
	protected:
		class elem
		{
		public:
			char m_data[sizeof(basetype)];
			elem* m_prev;
			elem* m_next;
		};

	public:
		class iterator
		{
			friend class listallocator;
		public:
			iterator()
			{
				m_elem=NULL;
			}

			iterator(basetype* i_elem)
			{
				m_elem=(elem*)i_elem;
			}

			iterator(elem* i_elem)
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

			bool operator==(const iterator& i_other) const
			{
				return m_elem==i_other.m_elem;
			}

			bool operator!=(const iterator& i_other) const
			{
				return m_elem!=i_other.m_elem;
			}

		protected:
			elem* m_elem;
		};
	public:
		listallocator();
		~listallocator();
		basetype* allocate();
		void deallocate(basetype*);
		void deallocateplace(basetype*);
		void deallocateall();
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


	protected:
		vector<elem*> m_allocated;
		elem* m_free;
		elem m_head;
		elem m_end;
		unsigned m_size;

		void allocfree();
	}; //class listallocator

	template <class basetype,int blocksize> 
	MLINLINE void listallocator<basetype,blocksize>::allocfree()
	{
		elem* newelem=(elem*)malloc(blocksize*sizeof(elem));

		for (int n=0;n<blocksize-1; ++n)
			newelem[n].m_next=&newelem[n+1];

		newelem[blocksize-1].m_next=NULL;
		m_free=newelem;
		m_allocated.push_back(newelem);
	}

	template <class basetype,int blocksize> 
	MLINLINE basetype* listallocator<basetype,blocksize>::allocate()
	{
		elem* newelem;
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

	template <class basetype,int blocksize> 
	MLINLINE basetype* listallocator<basetype,blocksize>::allocate_place()
	{
		elem* newelem;

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

	template <class basetype, int blocksize> 
	MLINLINE void listallocator<basetype,blocksize>::deallocate(basetype* i_data)
	{
		i_data->~basetype();
		deallocateplace(i_data);
	}

	template <class basetype, int blocksize> 
	MLINLINE void listallocator<basetype,blocksize>::deallocateplace(basetype* i_data)
	{
		elem* actelem=(elem*)i_data;

		actelem->m_next->m_prev=actelem->m_prev;
		actelem->m_prev->m_next=actelem->m_next;

		actelem->m_next=m_free;

		m_free=actelem;
		m_size--;
	}

	template <class basetype, int blocksize> 
	MLINLINE void listallocator<basetype,blocksize>::deallocateall()
	{
		if (!m_size)
			return;

		elem* ptr=m_head.m_next;

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

	template <class basetype,int blocksize>
	listallocator<basetype,blocksize>::listallocator()
	{
		m_end.m_prev=&m_head;
		m_end.m_next=NULL;
		m_head.m_prev=NULL;
		m_head.m_next=&m_end;
		m_free=NULL;
		m_size=0;
		allocfree();
	}

	template <class basetype, int blocksize>
	MLINLINE listallocator<basetype,blocksize>::~listallocator()
	{
		for (unsigned int n=0; n<m_allocated.size(); ++n)
			free(m_allocated[n]);
	}

	template <class basetype, int blocksize>
	MLINLINE unsigned listallocator<basetype,blocksize>::size() const
	{
		return m_size;
	}

	template<class basetype, int blocksize>
	MLINLINE void listallocator<basetype,blocksize>::moveafter(iterator i_elem, iterator i_after)
	{
		elem* e=i_elem.m_elem;
		elem* after=i_after.m_elem;
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

	template<class basetype, int blocksize>
	MLINLINE void listallocator<basetype,blocksize>::movebefore(iterator i_elem, iterator i_before)
	{
		elem* e=i_elem.m_elem;
		elem* before=i_before.m_elem;
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

} //namespace ctr
#endif//_listallocator_h_
