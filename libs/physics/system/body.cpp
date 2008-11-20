#include "body.h"
#include "physicssystem.h"
#include "utils/assert.h"
#include "physics\collision\system\contact.h"

	body_t::body_t()
	{
	}
	body_t::~body_t()
	{
	    while (this->contacts._head.next!=&this->contacts._end)
			physicssystem::ptr()->cm.erase_contact((contact_t*)this->contacts._head.next->elem);

        while (this->shapes._head.next!=&this->shapes._end)
            this->release_shape(this->shapes._head.next->elem);
//		while (this->shapes)
	//		this->release_shape(this->shapes);

	}

	const mtx4x3& body_t::get_pos() const
	{
		assertion(this->array_index!=uint16(-1));
		return physicssystem::ptr()->bodystate_array[this->is_static].pos[this->array_index];
	}

	const vec3& body_t::get_vel() const
	{
		assertion(this->array_index!=uint16(-1));
		return physicssystem::ptr()->bodystate_array[this->is_static].vel[this->array_index];
	}

	const vec3& body_t::get_rotvel() const
	{
		assertion(this->array_index!=uint16(-1));
		return physicssystem::ptr()->bodystate_array[this->is_static].rotvel[this->array_index];
	}


	void body_t::set_pos(const mtx4x3& i_pos)
	{
		assertion(this->array_index!=uint16(-1));
		physicssystem::ptr()->bodystate_array[this->is_static].pos[this->array_index]=i_pos;
	}

	void body_t::set_vel(const vec3& i_vel)
	{
		assertion(this->array_index!=uint16(-1));
		physicssystem::ptr()->bodystate_array[this->is_static].vel[this->array_index]=i_vel;
	}

	void body_t::set_rotvel(const vec3& i_rotvel)
	{
		assertion(this->array_index!=uint16(-1));
		physicssystem::ptr()->bodystate_array[this->is_static].rotvel[this->array_index]=i_rotvel;
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
        aabb aabb=transform(physicssystem::ptr()->bodystate_array[this->is_static].pos[this->array_index],s->bounding);
        s->collider=physicssystem::ptr()->broad_phase.create_object(s,aabb,this->is_static);

		return s;
	}

	void body_t::release_shape(shape_t* i_shape)
	{
		assertion(i_shape->body==this);
		this->shapes.erase(i_shape);
		delete i_shape;
	}
