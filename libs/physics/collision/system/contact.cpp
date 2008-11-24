#include "contact.h"

int get_contact_index(contact_t* contact, const vec3& relpos_body1, const vec3& relpos_body2)
{
    const float contact_threshhold=0.05f;
    int contact_index;
    for (contact_index=0; contact_index<contact->contact_count; ++contact_index)
    {
        float dist2;
        dist2=(contact->relpos[0][contact_index]-relpos_body1).squarelength();

        if (dist2<contact_threshhold*contact_threshhold)
            break;

        dist2=(contact->relpos[1][contact_index]-relpos_body2).squarelength();

        if (dist2<contact_threshhold*contact_threshhold)
            break;
    }

    return contact_index;
}

void contact_t::add_contact(const vec3& relpos_body1, const vec3& relpos_body2)
{
    int contact_index=get_contact_index(this,relpos_body1, relpos_body2);

    if (contact_index<this->contact_count)
    {
        //ha mar volt, akkor o lesz az
        this->relpos[0][contact_index]=relpos_body1;
        this->relpos[1][contact_index]=relpos_body2;
    }
    else
    {
        if (this->contact_count<MAX_CONTACTNUM_PER_BODYPAIR)
        {
            //ha meg nem volt, de van hely
            this->relpos[0][this->contact_count]=relpos_body1;
            this->relpos[1][this->contact_count]=relpos_body2;
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
