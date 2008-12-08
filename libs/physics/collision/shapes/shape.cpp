#include "shape.h"
#include "physics/system/physicssystem.h"

shape_t::~shape_t()
{
    contact_edge* contact,*next_contact;

    for (contact=body->contacts.first(); contact!=body->contacts.last();contact=next_contact)
    {
        next_contact=contact->next;

        if (contact->elem->shape[0]==this || contact->elem->shape[1]==this)
            physicssystem::ptr->contact_manager.erase_contact(contact->elem);
    }
}

