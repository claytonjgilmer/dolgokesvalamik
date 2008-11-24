#ifndef _constraint_h_
#define _constraint_h_

#include "containers/intr_list.h"

	struct body_t;
	struct constraint_t;
	struct constraint_edge:intr_list_node<struct constraint_t>
	{
		struct body_t* other;
	};

	struct constraint_t
	{
		struct body_t* body[2];
		struct constraint_edge edge[2];

		constraint_t(body_t* i_body1, body_t* i_body2)
		{
			body[0]=i_body1;
			body[1]=i_body2;
			edge[0].elem=edge[1].elem=this;
			edge[0].other=i_body2;
			edge[1].other=i_body1;
		}
	};
#endif//_constraint_h_
