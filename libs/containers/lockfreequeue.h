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
//			tc_interlockedExchange(&tailtmp, *(__int64*)&tail);

			long ret=interlocked_compare_exchange_32((long*)&tailtmp.node->next,(LONG)node,0);

			if (ret)
//			if (tc_interlockedCompareExchange(tailtmp.node->next,(int)node,0))
				break;

			count_t nexttmp={tailtmp.node->next,tailtmp.count+1};
			interlocked_compare_exchange_64((LONGLONG*)&tail,*(LONGLONG*)&nexttmp,*(LONGLONG*)&tailtmp);
//			tc_interlockedCompareExchange(&tail,(int)tailtmp->node->next,tailtmp.count+1,(int)tailtmp.node,tailtmp.count);
			spin_loop();

		}

		count_t tmp={node,tailtmp.count+1};
		interlocked_compare_exchange_64((LONGLONG*)&tail,*(LONGLONG*)&tmp,*(LONGLONG*)&tailtmp);
//		tc_interlockedCompareExchange( &tail, (int)node, tailtmp.count+1 , (int)tailtmp.node, tailtmp.count );
	}

	basetype* pop()
	{
		count_t ohead;
		count_t otail;

		while( 1 )
		{
			interlocked_exchange_64((LONGLONG*)&ohead,*(LONGLONG*)&head);
			interlocked_exchange_64((LONGLONG*)&otail,*(LONGLONG*)&tail);
//			tc_interlockedExchange( &ohead , *(__int64*)&head );
//			tc_interlockedExchange( &otail , *(__int64*)&tail );
			//
			basetype *next=ohead.node->next;

			// is queue empty
			if( ohead.node==otail.node)
			{
				// is it really empty
				if( !next )
					return 0;

				// or is just tail falling behind...
				count_t tmp={next,otail.count+1};
				interlocked_compare_exchange_64((LONGLONG*)&tail,*(LONGLONG*)&tmp,*(LONGLONG*)&otail);
//				tc_interlockedCompareExchange( &tail, (int)next , otail.count+1 , (int)otail.node, otail.count );
			}
			else
			{
				count_t tmp={next,ohead.count+1};
				if (interlocked_compare_exchange_64((LONGLONG*)&head,*(LONGLONG*)&tmp,*(LONGLONG*)&ohead))
//				if( tc_interlockedCompareExchange( &head , (int)next , ohead.count+1 , (int)ohead.node, ohead.count ) )
					return next;
			}

			spin_loop();
		}
	}

	count_t head,tail;
	char null_node[sizeof(basetype)];

};
#endif//_lockfreequeue_h_