#ifndef _contactgroupmanager_h_
#define _contactgroupmanager_h_

#include "physics\collision\system\contact.h"
#include "containers\vector.h"
#include "containers\listallocator.h"

struct contact_group
{
    int first_contact;
    int contact_count;

    int first_joint;
    int joint_count;
};

struct nbody_t;

struct contact_group_manager_t
{
    vector<contact_surface_t*> contact_array;
    vector<contact_group> group_array;



    void create_contact_groups(nbody_t& bodystate_array, list_allocator<sizeof(contact_surface_t)>& contact_list);
};

#endif//_contactgroupmanager_h_
