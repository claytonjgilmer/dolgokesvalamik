#include "body.h"
#include "physicssystem.h"
#include "utils/assert.h"
#include "physics\collision\system\contact.h"

body_t::body_t():
solver_stamp(-1),
solver_index(0)
{
}
	body_t::~body_t()
	{
	    while(this->contacts.first()!=this->contacts.last())
            physicssystem::ptr->contact_manager.erase_contact(this->contacts.first()->elem);

        while (this->shapes.first()!=this->shapes.last())
            this->release_shape(this->shapes.first());
//		while (this->shapes)
	//		this->release_shape(this->shapes);

	}

	const mtx4x3& body_t::get_pos() const
	{
		assertion(this->array_index!=uint16(-1));
		return physicssystem::ptr->bodystate_array.pos[this->array_index];
	}

	const vec3& body_t::get_vel() const
	{
		assertion(this->array_index!=uint16(-1));
		return physicssystem::ptr->bodystate_array.vel[this->array_index];
	}

	const vec3& body_t::get_rotvel() const
	{
		assertion(this->array_index!=uint16(-1));
		return physicssystem::ptr->bodystate_array.rotvel[this->array_index];
	}


	void body_t::set_pos(const mtx4x3& i_pos)
	{
		assertion(this->array_index!=uint16(-1));
		physicssystem::ptr->bodystate_array.pos[this->array_index]=i_pos;
	}

	void body_t::set_vel(const vec3& i_vel)
	{
		assertion(this->array_index!=uint16(-1));
		physicssystem::ptr->bodystate_array.vel[this->array_index]=i_vel;
	}

	void body_t::set_rotvel(const vec3& i_rotvel)
	{
		assertion(this->array_index!=uint16(-1));
		physicssystem::ptr->bodystate_array.rotvel[this->array_index]=i_rotvel;
	}

	shape_t* body_t::add_shape(const shape_desc& i_desc)
	{
		shape_t* s;

		switch (i_desc.type)
		{
		case shape_type_sphere:
			{
				s=new sphere_shape((const sphere_shape_desc&)i_desc);
				break;
			}
		case shape_type_box:
			{
				s=new box_shape((const box_shape_desc&)i_desc);
				break;
			}
		default:
            {
                s=NULL;
                assertion(0,"invalid shape type");
            }
		}

		s->body=this;
		this->shapes.push_front(s);
        aabb_t aabb=transform(physicssystem::ptr->bodystate_array.pos[this->array_index],s->bounding);
        s->collider=physicssystem::ptr->broad_phase.create_object(s,aabb,!this->array_index);

		return s;
	}

	void body_t::release_shape(shape_t* i_shape)
	{
		assertion(i_shape->body==this);
		this->shapes.erase(i_shape);
		physicssystem::ptr->broad_phase.release_object(i_shape->collider);
		delete i_shape;
	}
