#ifndef _contactmanager_h_
#define _contactmanager_h_

#include "contact.h"
#include "containers/listallocator.h"
#include "containers/vector.h"
#include "threading/mutex.h"

#define hashtable_size 1024
	struct contactmanager
	{
		contactmanager();
		contact_t* get_contact(body_t* i_body1, body_t* i_body2);
		void erase_contact(contact_t* i_contact);


		listallocator<contact_t> contact_list;
//		vector<contact_t*> contact_array;
		contact_t* contact_hash[hashtable_size];
		mutex cm;
	};
#endif//_contactmanager_h_
