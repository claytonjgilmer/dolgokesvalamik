#ifndef _constraint_h_
#define _constraint_h_

namespace physics
{
	struct constraint_t;
	struct constraint_edge
	{
		body_t* other;
		constraint_t* constraint;
		constraint_edge* prev;
		constraint_edge* next;
	};

	struct constraint_t
	{
		body_t* body1;
		body_t* body2;

		constraint_edge edge[2];

		constraint_t()
		{
			edge[0].constraint=edge[1].constraint=this;
		}
	};

}
#endif//_constraint_h_