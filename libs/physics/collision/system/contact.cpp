#include "contact.h"
#include "physics/system/physicssystem.h"

int get_contact_index(contact_t* contact, const vec3 relpos[2])
{
    const float contact_threshhold=0.05f;
    int contact_index;
    for (contact_index=0; contact_index<contact->contact_count; ++contact_index)
    {
        float dist2;
        dist2=(contact->contactarray[contact_index].rel_pos[0]-relpos[0]).squarelength();

        if (dist2<contact_threshhold*contact_threshhold)
            break;

        dist2=(contact->contactarray[contact_index].rel_pos[1]-relpos[1]).squarelength();

        if (dist2<contact_threshhold*contact_threshhold)
            break;
    }

    return contact_index;
}

void contact_t::add_contact(const vec3 relpos[][2], int contact_count,const vec3& normal_body1)
{
	int contact_count_tmp=this->contact_count;
	for (int n=0; n<contact_count; ++n)
	{
		int contact_index=get_contact_index(this,relpos[n]);

		if (contact_index<this->contact_count)
		{
			//ha mar volt, akkor o lesz az
			this->contactarray[contact_index].rel_pos[0]=relpos[n][0];
			this->contactarray[contact_index].rel_pos[1]=relpos[n][1];
		}
		else
		{
			this->contactarray[contact_count_tmp].rel_pos[0]=relpos[n][0];
			this->contactarray[contact_count_tmp].rel_pos[1]=relpos[n][1];
			++contact_count_tmp;
		}
	}

	//most kell kivalasztani egy nagy negyszoget;
	if (contact_count_tmp>MAX_CONTACTNUM_PER_BODYPAIR)
	{
		vec3 tmp; tmp.set(1,1,1);
		vec3 axis[2];
		axis[0].cross(normal_body1,tmp);
		axis[1].cross(normal_body1,axis[0]);
		float minv[2]={FLT_MAX,FLT_MAX};
		float maxv[2]={-FLT_MAX,-FLT_MAX};
		one_contact data[4];
		for (int n=0; n<contact_count_tmp; ++n)
		{
			float v=dot(axis[0],this->contactarray[n].rel_pos[0]);
			if (v<minv[0])
			{
				minv[0]=v;
				data[0]=this->contactarray[n];
			}

			if (v>maxv[0])
			{
				maxv[0]=v;
				data[2]=this->contactarray[n];
			}

			v=dot(axis[1],this->contactarray[n].rel_pos[0]);
			if (v<minv[1])
			{
				minv[1]=v;
				data[1]=this->contactarray[n];
			}

			if (v>maxv[1])
			{
				maxv[1]=v;
				data[3]=this->contactarray[n];
			}
		}

		for (int n=0; n<MAX_CONTACTNUM_PER_BODYPAIR; ++n)
			this->contactarray[n]=data[n];
	}
	this->contact_count=min(contact_count_tmp,MAX_CONTACTNUM_PER_BODYPAIR);
}

/*
    update:
    ki kell szedni az ervenytelen kontaktokat
    1. nempenetralo kontaktok
    2. tangensiranyban eltavolodott kontaktok
*/
void contact_t::update()
{
    physicssystem* ptr=physicssystem::ptr;
    const mtx4x3&body1_pos=ptr->bodystate_array[this->body[0]->is_static].pos[this->body[0]->array_index];
    const mtx4x3&body2_pos=ptr->bodystate_array[this->body[1]->is_static].pos[this->body[1]->array_index];
    for (int n=0; n<this->contact_count; ++n)
    {
        vec3 abs_pos[2];
        body1_pos.transform(abs_pos[0],this->contactarray[n].rel_pos[0]);
        body2_pos.transform(abs_pos[1],this->contactarray[n].rel_pos[1]);
        vec3 dir=abs_pos[1]-abs_pos[0];

        float penetration=dot(this->normal,dir);
        if (penetration>CONTACT_MIN_PENETRATION)
        {
            this->contactarray[n]=this->contactarray[--this->contact_count];
        }
        else
        {
            vec3 tangentdir=dir-penetration*this->normal;
            if (tangentdir.squarelength()>CONTACT_MAX_TANGENTDIST*CONTACT_MAX_TANGENTDIST)
                this->contactarray[n]=this->contactarray[--this->contact_count];
        }
    }

//    if (!this->contact_count)
//    {
//        physicssystem::ptr->contact_manager.erase_contact(this);
//    }
}



