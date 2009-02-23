#ifndef _list_allocator_h_
#define _list_allocator_h_

#include "vector.h"

	template <int elemsize, int blocksize=128> struct list_allocator //blocksize kettohatvany!
	{
		struct elem_t
		{
			char m_data[elemsize];
			elem_t* m_prev;
			elem_t* m_next;
		};

		struct iterator
		{
			iterator()
			{
				m_elem=NULL;
			}

			iterator(elem_t* i_elem)
			{
				m_elem=i_elem;
			}

			void* operator*()
			{
				return m_elem->m_data;
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

		iterator begin(){return iterator(m_head.m_next);}
		iterator end(){return iterator(&m_end);}

		list_allocator();
		~list_allocator();
//		elemsize* allocate();
//		void deallocate(elemsize*);
		void deallocate_place(void*);
		void* allocate_place();

		unsigned size() const;

		vector<elem_t*> m_allocated;
		elem_t* m_free;
		elem_t  m_head;
		elem_t  m_end;
		unsigned m_size;

		void allocfree();
	}; //struct  list_allocator

	template <int elemsize,int blocksize>
	MLINLINE void list_allocator<elemsize,blocksize>::allocfree()
	{
		elem_t* newelem=(elem_t*)malloc(blocksize*sizeof(elem_t));

		for (int n=0;n<blocksize-1; ++n)
			newelem[n].m_next=&newelem[n+1];

		newelem[blocksize-1].m_next=NULL;
		m_free=newelem;
		m_allocated.push_back(newelem);
	}

	template <int elemsize,int blocksize>
	MLINLINE void* list_allocator<elemsize,blocksize>::allocate_place()
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

		return (&newelem->m_data);
	}

	template <int elemsize, int blocksize>
	MLINLINE void list_allocator<elemsize,blocksize>::deallocate_place(void* i_data)
	{
		elem_t* actelem=(elem_t*)i_data;

		actelem->m_next->m_prev=actelem->m_prev;
		actelem->m_prev->m_next=actelem->m_next;

		actelem->m_next=m_free;

		m_free=actelem;
		m_size--;
	}


	template <int elemsize,int blocksize>
	list_allocator<elemsize,blocksize>::list_allocator()
	{
		m_end.m_prev=&m_head;
		m_end.m_next=NULL;
		m_head.m_prev=NULL;
		m_head.m_next=&m_end;
		m_free=NULL;
		m_size=0;
		allocfree();
	}

	template <int elemsize, int blocksize>
	MLINLINE list_allocator<elemsize,blocksize>::~list_allocator()
	{
		for (unsigned int n=0; n<m_allocated.size(); ++n)
			free(m_allocated[n]);
	}

	template <int elemsize, int blocksize>
	MLINLINE unsigned list_allocator<elemsize,blocksize>::size() const
	{
		return m_size;
	}

#endif//_list_allocator_h_


#ifndef _pool_allocator_h_
#define _pool_allocator_h_

#include "threading/mutex.h"

	template<int elemsize>
	struct pool_elem_t
	{
		char m_data[elemsize];
		unsigned block_index;
	};
	template<int elemsize, int elemcount> struct pool_chunk
	{
		pool_elem_t<elemsize> elem_array[elemcount];
		unsigned short m_free_stack[elemcount];
		unsigned short freecount;

		pool_chunk(unsigned i_block_index)
		{
			freecount=elemcount;

			for (int n=0; n<elemcount; ++n)
			{
				m_free_stack[n]=elemcount-1-n;
				elem_array[n].block_index=i_block_index;
			}
		}

		void set_block_index(unsigned i_block_index)
		{
			for (int n=0; n<elemcount; ++n)
				elem_array[n].block_index=i_block_index;
		}

		void* allocate()
		{
			assert(freecount>0);
			return elem_array+m_free_stack[--freecount];
		}

		void deallocate(void* ptr)
		{
			unsigned short index=(unsigned short)(((pool_elem_t<elemsize>*) ptr)-elem_array);
			assert(index>=0 && index<elemcount);

			m_free_stack[freecount++]=index;
		}

		bool is_empty() const
		{
			return freecount==elemcount;
		}

		bool is_full() const
		{
			return freecount==0;
		}
	};

	struct allocator_list_elem
	{
		allocator_list_elem* prev, * next;

		allocator_list_elem()
		{
			prev=NULL;
			next=s_allocator_pool_head;

			if (s_allocator_pool_head)
				s_allocator_pool_head->prev=this;

			s_allocator_pool_head=this;
		}

		~allocator_list_elem()
		{
			if (prev)
				prev->next=next;
			else
				s_allocator_pool_head=next;

			if (next)
				next->prev=prev;
		}

		virtual void garbage_collection()=0;
		static allocator_list_elem* s_allocator_pool_head;
	};

	template <int elemsize, int elemcount=128> struct pool_allocator:allocator_list_elem
	{
		mutex cs;
		vector<pool_chunk<elemsize,elemcount>*> chunk_array;
		unsigned first_free_chunk;

		pool_allocator()
		{
			first_free_chunk=-1;
			chunk_array.reserve(8);
		}
		~pool_allocator()
		{
			for (unsigned n=0; n<chunk_array.size(); ++n)
				delete chunk_array[n];
		}

		void garbage_collection()
		{
			for (unsigned n=0; n<chunk_array.size(); ++n)
			{
				if (chunk_array[n]->is_empty())
				{
					delete chunk_array[n];
					chunk_array[n]=chunk_array.back();
					chunk_array.pop_back();

					if (n<chunk_array.size())
						chunk_array[n]->set_block_index(n);
					--n;
				}
			}

			first_free_chunk=-1;
			for (unsigned n=0; n<chunk_array.size(); ++n)
			{
				if (!chunk_array[n]->is_full())
				{
					first_free_chunk=n;
					break;
				}
			}
		}

		void* alloc()
		{
			if (first_free_chunk==-1)
			{
				first_free_chunk=chunk_array.size();
				chunk_array.push_back(new pool_chunk<elemsize,elemcount>(first_free_chunk));
			}

			pool_chunk<elemsize,elemcount>* act_chunk=chunk_array[first_free_chunk];

			void* ptr=act_chunk->allocate();

			if (act_chunk->is_full())
			{
				int f=first_free_chunk+1;
				first_free_chunk=-1;
				for (int n=f;n<chunk_array.size(); ++n)
				{
					if (!chunk_array[n]->is_full())
					{
						first_free_chunk=n;
						break;
					}
				}
			}

			return ptr;
		}
		void free(void* ptr)
		{
			int block_index=((pool_elem_t<elemsize>*)ptr)->block_index;

			chunk_array[block_index]->deallocate(ptr);
			if (block_index<first_free_chunk)
				first_free_chunk=block_index;
		}
	}; //struct  pool_allocator

#define DECLARE_ALLOCATOR(_TYPE_)  pool_allocator<_TYPE_> s_allocator; \
	void* new(size_t size) { return s_allocator.allocate_place();}\
	void delete(void* elem){ s_allocator.deallocate_place((_TYPE*)elem);}

#define DEFINE_ALLOCATOR(_TYPE_) pool_allocator<_TYPE_> _TYPE_::s_allocator;

#endif//_pool_allocator_h_


