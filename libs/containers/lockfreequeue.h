#ifndef _lockfreequeue_h_
#define _lockfreequeue_h_

#include <intrin.h>
#include "threading/interlocked.h"

template<typename basetype>
struct lockfree_queue_t
{
	struct count_t
	{
		basetype* node;
		uint32 count;
	};

	lockfree_queue_t()
	{
		((basetype*)null_node)->next=0;
		head.node=tail.node=(basetype*)null_node;
		head.count=tail.count=0;
	}

	~lockfree_queue_t()
	{
	}

	void push(basetype* node)
	{
		volatile count_t tailtmp;
		node->next=0;

		for (;;)
		{
			interlocked_exchange_64((LONGLONG*)&tailtmp,*(LONGLONG*)&tail);

#if 0
			long ret=interlocked_compare_exchange_32((long*)&tailtmp.node->next,(LONG)node,0);

			if (ret)
				break;
#else
			if (!interlocked_compare_exchange_32((long*)&tailtmp.node->next,(LONG)node,0))
				break;
#endif
			count_t nexttmp={tailtmp.node->next,tailtmp.count+1};
			interlocked_compare_exchange_64((LONGLONG*)&tail,*(LONGLONG*)&nexttmp,*(LONGLONG*)&tailtmp);
			spin_loop();

		}

		count_t tmp={node,tailtmp.count+1};
		interlocked_compare_exchange_64((LONGLONG*)&tail,*(LONGLONG*)&tmp,*(LONGLONG*)&tailtmp);
	}

	basetype* pop()
	{
		count_t ohead;
		count_t otail;

		while( 1 )
		{
			interlocked_exchange_64((LONGLONG*)&ohead,*(LONGLONG*)&head);
			interlocked_exchange_64((LONGLONG*)&otail,*(LONGLONG*)&tail);
			basetype *next=ohead.node->next;

			if( ohead.node==otail.node)
			{
				if( !next )
					return 0;

				count_t tmp={next,otail.count+1};
				interlocked_compare_exchange_64((LONGLONG*)&tail,*(LONGLONG*)&tmp,*(LONGLONG*)&otail);
			}
			else
			{
				count_t tmp={next,ohead.count+1};
				if (interlocked_compare_exchange_64((LONGLONG*)&head,*(LONGLONG*)&tmp,*(LONGLONG*)&ohead))
					return next;
			}

			spin_loop();
		}
	}

	count_t head,tail;
	char null_node[sizeof(basetype)];

};

template<typename basetype,uint32 capacity>
struct lockfree_array_queue_t
{
	basetype* ptr_buf[capacity];
	uint32 used[capacity];
	uint32 tail_index,head_index;

	lockfree_array_queue_t()
	{
		tail_index=head_index=0;
		memset(used,0,capacity*sizeof(uint32));
	}

	basetype* pop()
	{
		long tail_index_tmp;
		interlocked_exchange_32(&tail_index_tmp,tail_index);
		if (interlocked_compare_exchange_32((long*)&used[tail_index_tmp & (capacity-1)],0,1) == 1)
		{
			basetype* ret=ptr_buf[tail_index_tmp & (capacity-1)];
			interlocked_add_32((long*)&tail_index,1);
			return ret;
		}
		else
		{
			return 0;
		}

	}

	void push(basetype* item)
	{
		while (1)
		{
			long head_index_tmp;
			interlocked_exchange_32(&head_index_tmp,head_index);
			if (interlocked_compare_exchange_32((long*)&used[head_index_tmp & (capacity-1)],2,0) == 0)
			{
				interlocked_add_32((long*)&head_index,1);
				ptr_buf[head_index_tmp & (capacity-1)]=item;
				interlocked_exchange_32((long*)&used[head_index_tmp & (capacity-1)],1);
				return;
			}
			spin_loop();
		}
	}

};


#endif//_lockfreequeue_h_