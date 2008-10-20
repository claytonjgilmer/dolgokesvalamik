#ifndef _constraint_h_
#define _constraint_h_

namespace physics
{
	struct body_t;
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
		body_t* body[2];
		constraint_edge edge[2];

		constraint_t(body_t* i_body1, body_t* i_body2)
		{
			body[0]=i_body1;
			body[2]=i_body2;
			edge[0].constraint=edge[1].constraint=this;
			edge[0].other=i_body2;
			edge[1].other=i_body1;
		}
	};

}
#endif//_constraint_h_