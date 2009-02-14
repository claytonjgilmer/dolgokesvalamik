#include "contactmanager.h"
#include "physics/system/physicssystem.h"

static int hash_check[hashtable_size];

	contactmanager::contactmanager()
	{
//		ZeroMemory(this->contact_hash,sizeof(this->contact_hash));

		ZeroMemory(hash_check,sizeof(hash_check));
	}

	uint32 get_hashindex(body_t* i_body1, body_t* i_body2)
	{
#if 0
		uint32 key=((uint32)i_body1) ^ ((uint32)i_body2);
//		const unsigned h = 2376512323u;
		const unsigned int fnv_prime = 0x811C9DC5;
		return (key ^ fnv_prime) & (hashtable_size-1);
#else
		uint32 key=((uint32)i_body1) ^ ((uint32)i_body2);
		unsigned int hash = 1315423911;
		hash ^= ((hash << 5) + key + (hash >> 2));
		hash ^= ((hash << 5) + key + (hash >> 2));
		hash ^= ((hash << 5) + key + (hash >> 2));
		hash ^= ((hash << 5) + key + (hash >> 2));
		hash ^= ((hash << 5) + key + (hash >> 2));
		return hash & (hashtable_size-1);
#endif
	}

	contact_t* contactmanager::get_contact(body_t* i_body1, body_t* i_body2)
    {
		uint32 index=get_hashindex(i_body1,i_body2);

		blocklocker bl(this->cm);
//		contact_t* ptr=this->contact_hash[index];
		contact_edge* ptr;
		for (ptr=i_body1->contacts.first(); ptr!=i_body1->contacts.last() && ptr->other!=i_body2; ptr=ptr->next);

		if (ptr!=i_body1->contacts.last())
			return ptr->elem;

		++hash_check[index];
		contact_t* c=this->contact_list.allocate_place();
		new (c) contact_t(i_body1,i_body2);

//			ptr->prev=0;
//			ptr->next=this->contact_hash[index];
//			if (this->contact_hash[index])
//				this->contact_hash[index]->prev=ptr;

//			this->contact_hash[index]=ptr;

		return c;
	}

	void contactmanager::erase_contact(contact_t* i_contact)
	{
#if 0
		if (i_contact->next)
			i_contact->next->prev=i_contact->prev;

		if (i_contact->prev)
		{
			i_contact->prev->next=i_contact->next;
		}
		else //ha nincs elozoje, akkor o az elso a hashtablaban
		{
			uint32 index=get_hashindex(i_contact->shape[0],i_contact->shape[1]);
			this->contact_hash[index]=this->contact_hash[index]->next;
		}
#endif
		contact_list.deallocate(i_contact);
	}


	void contactmanager::update_contacts()
	{
	    list_allocator<contact_t>::iterator it,next_it;

	    for (it=this->contact_list.begin(); it!=this->contact_list.end(); it=next_it)
	    {
			next_it=it;
			++next_it;
            (*it).update();
            if (!(*it).contact_count)
                erase_contact(&(*it));
	    }
	}
