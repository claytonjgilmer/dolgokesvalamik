#ifndef _contact_h_
#define _contact_h_

#include "constraint.h"
#include "math/vec3.h"
#include "physics/system/body.h"
#include "containers/c_array.h"

#define MAX_CONTACTNUM_PER_BODYPAIR 4
#define CONTACTBUFFER_SIZE 12

struct contact_point_t
{
	vec3 rel_pos[2];
	vec3 abs_pos[2];
	f32 penetration;
	f32 cached_lambda;

	contact_point_t():cached_lambda(0){}
};

#define CONTACT_MIN_PENETRATION 0.01f
#define CONTACT_MAX_TANGENTDIST 0.05f

struct contact_surface_t
{
    body_t* body[2];
    contact_edge edge[2];
    c_array<contact_point_t,MAX_CONTACTNUM_PER_BODYPAIR> contactarray;
    vec3 normal;

    int group_index;

	f32 friction;
	f32 restitution;
	char contact_count;
	char updated;

    contact_surface_t(body_t* i_body1, body_t* i_body2);
    ~contact_surface_t();

    void add_contact(const vec3 relpos[][2], int new_contact_count, const vec3& normal_body1);
    void update();
};





MLINLINE contact_surface_t::contact_surface_t(body_t* i_body1, body_t* i_body2)
{
    this->body[0]=i_body1;
    this->body[1]=i_body2;
    this->edge[0].elem=this->edge[1].elem=this;
    this->edge[0].other=i_body2;
    this->edge[1].other=i_body1;

    //befuzzuk a ket test kontakt-listajaba
    this->body[0]->contacts.push_front(this->edge);
    this->body[1]->contacts.push_front(this->edge+1);
    this->contact_count=0;
	this->updated=0;

//    for (int n=0; n<CONTACTBUFFER_SIZE; ++n)
//    	this->contactarray[n].cached_lambda=0;
}

MLINLINE contact_surface_t::~contact_surface_t()
{
    //kifuzzuk a ket test kontakt-listajabol
    this->body[0]->contacts.erase(this->edge);
    this->body[1]->contacts.erase(this->edge+1);
}

#endif//_contact_h_
