#ifndef _contact_h_
#define _contact_h_

#include "constraint.h"
#include "math/vec3.h"
#include "physics/system/body.h"

#define MAX_CONTACTNUM_PER_BODYPAIR 4
#define CONTACTBUFFER_SIZE 8

struct one_contact
{
	vec3 rel_pos[2];
	float cached_lambda;
};

#define CONTACT_MIN_PENETRATION 0.01f
#define CONTACT_MAX_TANGENTDIST 0.05f

struct contact_t
{
    shape_t* shape[2];
    contact_edge edge[2];
    one_contact contactarray[CONTACTBUFFER_SIZE];
    vec3 normal;
    contact_t* prev,*next;

    char contact_count;
    int group_index;

    contact_t(shape_t* i_shape1, shape_t* i_shape2);
    ~contact_t();

    void add_contact(const vec3 relpos[][2], int contact_count, const vec3& normal_body1);
    void update();
};




MLINLINE contact_t::contact_t(shape_t* i_shape1, shape_t* i_shape2)
{
    this->shape[0]=i_shape1;
    this->shape[1]=i_shape2;
    this->edge[0].elem=this->edge[0].elem=this;
    this->edge[0].other=i_shape2->body;
    this->edge[1].other=i_shape1->body;

    //befuzzuk a ket test kontakt-listajaba
    i_shape1->body->contacts.push_front(this->edge);
    i_shape2->body->contacts.push_front(this->edge+1);
    this->contact_count=0;

    for (int n=0; n<CONTACTBUFFER_SIZE; ++n)
    	this->contactarray[n].cached_lambda=0;
}

MLINLINE contact_t::~contact_t()
{
    //kifuzzuk a ket test kontakt-listajabol
    this->shape[0]->body->contacts.erase(this->edge);
    this->shape[1]->body->contacts.erase(this->edge+1);
}

#endif//_contact_h_
