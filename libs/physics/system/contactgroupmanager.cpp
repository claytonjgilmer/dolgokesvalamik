#include "contactgroupmanager.h"

    void create_one_group(contact_group_manager_t* gm, body_t* b, int group_index);
    void contact_group_manager_t::create_contact_groups(body_t* dynbody_array[], int dynbody_count,list_allocator<contact_t>& contact_list)
    {
        for (int n=0; n<dynbody_count; ++n)
            dynbody_array[n]->group_index=-1;

        list_allocator<contact_t>::iterator cit;
        for (cit=contact_list.begin(); cit!=contact_list.end();++cit)
            (*cit)->group_index=-1;

        int group_count=0;
        contact_array.resize(contact_list.size());
        group_array.resize(0);
        int act_contact_count=0;
        group_array.push_back(contact_group());

        for (int n=0; n<dynbody_count; ++n)
        {
            body_t* b=dynbody_array[n];

            if (b->group_index==-1 && b->contacts.begin()!=b->contacts.end())
            {
                contact_group* group=&group_array.back();
                group->contact_count=0;
                group->first_contact=act_contact_count;
                create_one_group(this,b,group_count);

                if (group->contact_count)
                {
                    act_contact_count+=group->contact_count;
                    ++group_count;
                    group_array.push_back(contact_group());
                }
            }
        }
    }

    void create_one_group(contact_group_manager_t* gm, body_t* b, int group_index)
    {
        b->group_index=group_index;
        intr_list::iterator cit;
        for (cit=b->contacts.begin(); cit!=b->contacts.end(); ++cit)
        {
            constraint_edge* e=(constraint_edge*)(*cit);
            contact_t* c=(contact_t*)e->elem;
            if (c->group_index==-1)
            {
                contact_group* group=&gm->group_array[group_index];
                c->group_index=group_index;
                gm->contact_array[group->first_contact+group->contact_count]=c;
                ++group->contact_count;
                if (!e->other->is_static && e->other->group_index==-1)
                    create_one_group(gm,e->other,group_index);
            }
        }
    }

