#include "contactgroupmanager.h"
#include "nbody.h"

    void create_one_group(contact_group_manager_t* gm, body_t* b, int group_index);
    void contact_group_manager_t::create_contact_groups(nbody_t& bodystate_array, list_allocator<sizeof(contact_surface_t)>& contact_list)
    {
        for (uint32 n=1; n<bodystate_array.get_size(); ++n)
            bodystate_array.get_body(n)->group_index=-1;

		if (!contact_list.size())
			return;


        list_allocator<sizeof(contact_surface_t)>::iterator cit;
        for (cit=contact_list.begin(); cit!=contact_list.end();++cit)
            ((contact_surface_t*)(*cit))->group_index=-1;

        int group_count=0;
        this->contact_array.resize(contact_list.size());
        this->group_array.resize(0);
        int act_contact_count=0;
        this->group_array.push_back(contact_group());

        for (uint32 n=1; n<bodystate_array.get_size(); ++n)
        {
            body_t* b=bodystate_array.get_body(n);

            if (b->group_index==-1 && b->contacts.first())
            {
                contact_group* group=&group_array.back();
                group->contact_count=0;
                group->first_contact=act_contact_count;
                create_one_group(this,b,group_count);

                if (group->contact_count)
                {
                    act_contact_count+=group->contact_count;
                    ++group_count;
                    this->group_array.push_back(contact_group());
                }
            }
        }
    }

    void create_one_group(contact_group_manager_t* gm, body_t* b, int group_index)
    {
        b->group_index=group_index;
        contact_edge* e;
        for (e=b->contacts.first(); e; e=e->next)
        {
            contact_surface_t* c=e->elem;
            if (c->group_index==-1)
            {
                contact_group* group=&gm->group_array[group_index];
                c->group_index=group_index;
                gm->contact_array[group->first_contact+group->contact_count]=c;
                ++group->contact_count;
                if (e->other->array_index && e->other->group_index==-1)
                    create_one_group(gm,e->other,group_index);
            }
        }
    }

