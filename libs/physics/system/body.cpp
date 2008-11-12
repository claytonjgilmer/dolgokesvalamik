#include "body.h"
#include "physicssystem.h"
#include "utils/assert.h"

	body_t::body_t():
	contacts(NULL),
	joints(NULL),
	shapes(NULL)
	{
	}

	body_t::~body_t()
	{
		while (this->contacts)
			physicssystem::ptr()->cm.erase_contact((contact_t*)this->contacts->constraint);

		while (this->shapes)
			release_shape(this->shapes);

	}

	const mtx4x3& body_t::get_pos() const
	{
		assertion(array_index!=uint16(-1));
		return physicssystem::ptr()->bodystate_array[is_static].pos[array_index];
	}

	const vec3& body_t::get_vel() const
	{
		assertion(array_index!=uint16(-1));
		return physicssystem::ptr()->bodystate_array[is_static].vel[array_index];
	}

	const vec3& body_t::get_rotvel() const
	{
		assertion(array_index!=uint16(-1));
		return physicssystem::ptr()->bodystate_array[is_static].rotvel[array_index];
	}


	void body_t::set_pos(const mtx4x3& i_pos)
	{
		assertion(array_index!=uint16(-1));
		physicssystem::ptr()->bodystate_array[is_static].pos[array_index]=i_pos;
	}

	void body_t::set_vel(const vec3& i_vel)
	{
		assertion(array_index!=uint16(-1));
		physicssystem::ptr()->bodystate_array[is_static].vel[array_index]=i_vel;
	}

	void body_t::set_rotvel(const vec3& i_rotvel)
	{
		assertion(array_index!=uint16(-1));
		physicssystem::ptr()->bodystate_array[is_static].rotvel[array_index]=i_rotvel;
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
			assertion(0,"invalid shape type");
		}

		s->body=this;
		s->next=this->shapes;
		s->prev=0;

		if (this->shapes)
			this->shapes->prev=s;

		this->shapes=s;

		aabb aabb=transform(physicssystem::ptr()->bodystate_array[this->is_static].pos[this->array_index],s->bounding);
		s->collider=physicssystem::ptr()->broad_phase.create_object(s,aabb,this->is_static);

		return s;
	}

	void body_t::release_shape(shape_t* i_shape)
	{
		assertion(i_shape->body==this);

		if (i_shape->next)
			i_shape->next->prev=i_shape->prev;

		if (i_shape->prev)
			i_shape->prev->next=i_shape->next;

		if (i_shape==this->shapes)
			this->shapes=this->shapes->next;

		delete i_shape;
	}
