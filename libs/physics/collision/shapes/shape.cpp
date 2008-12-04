#include "shape.h"
#include "physics/system/physicssystem.h"

shape_t::~shape_t()
{
    while (this->contacts.first()!=this->contacts.last())
        physicssystem::ptr->contact_manager.erase_contact((contact_t*)(this->contacts.first())->elem);
}

