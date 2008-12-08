#include "contactmanager.h"
#include "physics/system/physicssystem.h"

static int hash_check[hashtable_size];

	contactmanager::contactmanager()
	{
		ZeroMemory(this->contact_hash,sizeof(this->contact_hash));

		ZeroMemory(hash_check,sizeof(hash_check));
	}

	uint32 get_hashindex(shape_t* i_body1, shape_t* i_body2)
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

	contact_t* contactmanager::get_contact(shape_t* i_shape1, shape_t* i_shape2)
    {
		uint32 index=get_hashindex(i_shape1,i_shape2);

		this->cm.lock();
		contact_t* ptr=this->contact_hash[index];

		while (ptr && (ptr->shape[0]!=i_shape1 || ptr->shape[1]!=i_shape2))
		{
		    assertion(ptr->shape[0]!=i_shape2 || ptr->shape[1]!=i_shape1);
			ptr=ptr->next;
		}

		if (!ptr)
		{
			++hash_check[index];
			ptr=this->contact_list.allocate_place();
			new (ptr) contact_t(i_shape1,i_shape2);

			ptr->prev=0;
			ptr->next=this->contact_hash[index];
			if (this->contact_hash[index])
				this->contact_hash[index]->prev=ptr;

			this->contact_hash[index]=ptr;
		}

        this->cm.unlock();
		return ptr;
	}

	void contactmanager::erase_contact(contact_t* i_contact)
	{
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

		contact_list.deallocate(i_contact);
	}


	void contactmanager::update_contacts()
	{
	    list_allocator<contact_t>::iterator it;

	    for (it=this->contact_list.begin(); it!=this->contact_list.end(); ++it)
	    {
            (*it)->update();
            if (!(*it)->contact_count)
                erase_contact((*it));
	    }
	}
