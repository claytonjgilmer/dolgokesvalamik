#ifndef _contact_h_
#define _contact_h_

#include "constraint.h"
#include "math/vec3.h"
#include "physics/system/body.h"

#define MAX_CONTACTNUM_PER_BODYPAIR 4

	struct contact_t:public constraint_t
	{
		vec3 relpos[2][MAX_CONTACTNUM_PER_BODYPAIR];
		vec3 normal;
		contact_t* prev,*next;

		char contactnum;

		contact_t(body_t* i_body1, body_t* i_body2):
		constraint_t(i_body1,i_body2)
		{
			//befuzzuk a ket test kontakt-listajaba
			this->edge[0].prev=0;
			this->edge[0].next=this->body[0]->contacts;
			if (this->body[0]->contacts)
				this->body[0]->contacts->prev=this->edge+0;
			this->body[0]->contacts=this->edge+0;

			this->edge[1].prev=0;
			this->edge[1].next=this->body[1]->contacts;
			if (this->body[1]->contacts)
				this->body[1]->contacts->prev=this->edge+1;
			this->body[1]->contacts=this->edge+1;

			this->contactnum=0;
		}

		~contact_t()
		{
			//kifuzzuk a ket test kontakt-listajabol
			if (this->edge[0].prev)
				this->edge[0].prev->next=this->edge[0].next;

			if (this->edge[0].prev)
				this->edge[0].prev->next=this->edge[0].next;

			if (this->body[0]->contacts=this->edge+0)
				this->body[0]->contacts=this->body[0]->contacts->next;

			if (this->edge[1].prev)
				this->edge[1].prev->next=this->edge[1].next;

			if (this->edge[1].prev)
				this->edge[1].prev->next=this->edge[1].next;

			if (this->body[1]->contacts=this->edge+1)
				this->body[1]->contacts=this->body[1]->contacts->next;
		}
	};
#endif//_contact_h_
