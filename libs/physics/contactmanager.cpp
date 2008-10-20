#include "contactmanager.h"

namespace physics
{
	contactmanager::contactmanager()
	{
		ZeroMemory(this->contact_hash,sizeof(this->contact_hash));
	}

	uint32 get_hashindex(uint32 i_key)
	{
		return i_key & (hashtable_size-1);
	}

	contact_t* contactmanager::get_contact(body_t* i_body1, body_t* i_body2)
	{
		if (i_body1>i_body2)
			math::swap(i_body1,i_body2);


		uint32 key=((uint32)i_body1) ^ ((uint32)i_body2);
		uint32 index=get_hashindex(key);

		contact_t* ptr=this->contact_hash[index];

		while (ptr && (ptr->body[0]!=i_body1 || ptr->body[1]!=i_body2))
			ptr=ptr->next;

		if (!ptr)
		{
			ptr=this->contact_list.allocate_place();
			new (ptr) contact_t(i_body1,i_body2);

			ptr->prev=0;
			ptr->next=this->contact_hash[index];
			if (this->contact_hash[index])
				this->contact_hash[index]->prev=ptr;

			this->contact_hash[index]=ptr;
//			this->contact_array.push_back(ptr);
		}

		return ptr;
	}

	void contactmanager::erase_contact(contact_t* i_contact)
	{

	}
}//namespace