#ifndef _contact_h_
#define _contact_h_

#include "constraint.h"

#define MAX_CONTACTNUM_PER_BODYPAIR 4

namespace physics
{
	struct contact_t:public constraint_t
	{
		math::vec3 relpos[2][MAX_CONTACTNUM_PER_BODYPAIR];
		math::vec3 normal;

		char contactnum;

		contact_t(body_t* i_body1, body_t* i_body2):
		constraint_t(i_body1,i_body2)
		{
			edge[0].prev=0;
			edge[0].next=body[0]->contacts;
			if (body[0]->contacts)
				body[0]->contacts->prev=edge+0;
			body[0]->contacts=edge+0;

			edge[1].prev=0;
			edge[1].next=body[1]->contacts;
			if (body[1]->contacts)
				body[1]->contacts->prev=edge+1;
			body[1]->contacts=edge+1;
		}
	};
}//namespace
#endif//_contact_h_