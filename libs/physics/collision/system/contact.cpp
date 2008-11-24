#include "contact.h"

int get_contact_index(contact_t* contact, const vec3 relpos[2])
{
    const float contact_threshhold=0.05f;
    int contact_index;
    for (contact_index=0; contact_index<contact->contact_count; ++contact_index)
    {
        float dist2;
        dist2=(contact->relpos[contact_index][0]-relpos[0]).squarelength();

        if (dist2<contact_threshhold*contact_threshhold)
            break;

        dist2=(contact->relpos[contact_index][1]-relpos[1]).squarelength();

        if (dist2<contact_threshhold*contact_threshhold)
            break;
    }

    return contact_index;
}

void contact_t::add_contact(const vec3 relpos[][2], int contact_count)
{
	for (int n=0; n<contact_count; ++n)
	{
		int contact_index=get_contact_index(this,relpos);

		if (contact_index<this->contact_count)
		{
			//ha mar volt, akkor o lesz az
			relpos[n][0]=this->relpos[contact_index][0];
			relpos[n][1]=this->relpos[contact_index][1];
		}
		else
		{
			if (this->contact_count<MAX_CONTACTNUM_PER_BODYPAIR)
			{
				//ha meg nem volt, de van hely
				this->relpos[this->contact_count][0]=relpos[0];
				this->relpos[this->contact_count][1]=relpos[1];
				++this->contact_count;
			}
			else
			{
				//ha meg nem volt, es nincs is mar hely, akkor egyet kibaszunk
				//az uj kontakt mindenkeppen megmarad
				//meg kellene talalni a legnagyobb teruletu negyszoget
				//faszomba man
				float area[4];
			}
		}
	}
}
