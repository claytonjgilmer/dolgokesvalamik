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

	contact_surface_t* contactmanager::get_contact(body_t* i_body1, body_t* i_body2)
    {
//		uint32 index=get_hashindex(i_body1,i_body2);

		blocklocker bl1(i_body1->contact_lock);
		blocklocker bl2(i_body2->contact_lock);
//		contact_surface_t* ptr=this->contact_hash[index];
		contact_edge* ptr;
		for (ptr=i_body1->contacts.first(); ptr && ptr->other!=i_body2; ptr=ptr->next);

		if (ptr)
			return ptr->elem;

//		++hash_check[index];
		

//			ptr->prev=0;
//			ptr->next=this->contact_hash[index];
//			if (this->contact_hash[index])
//				this->contact_hash[index]->prev=ptr;

//			this->contact_hash[index]=ptr;

		return new (this->contact_list.allocate_place()) contact_surface_t(i_body1,i_body2);;
	}

	void contactmanager::erase_contact(contact_surface_t* i_contact)
	{
		contact_list.deallocate_place(i_contact);
		i_contact->~contact_surface_t();
	}


	void contactmanager::update_contacts()
	{
	    list_allocator<sizeof(contact_surface_t)>::iterator it,next_it;

	    for (it=this->contact_list.begin(); it!=this->contact_list.end(); it=next_it)
	    {
			next_it=it;
			++next_it;
			contact_surface_t* contact_surface=(contact_surface_t*)*it;
            contact_surface->update();
            if (!contact_surface->contact_count)
                erase_contact(contact_surface);
	    }
	}

#include "render/rendersystem.h"

	void contactmanager::draw_contacts()
	{
		list_allocator<sizeof(contact_surface_t)>::iterator it,next_it;

		for (it=this->contact_list.begin(); it!=this->contact_list.end(); it=next_it)
		{
			next_it=it;
			++next_it;
			contact_surface_t* contact_surface=(contact_surface_t*)*it;
			contact_surface->update();

			for (int n=0; n<contact_surface->contact_count; ++n)
			{
				contact_point_t* cp=contact_surface->contactarray+n;

				rendersystem::ptr->draw_circle(cp->abs_pos[0],0.3f);
			}
			if (!contact_surface->contact_count)
				erase_contact(contact_surface);
		}
	}
