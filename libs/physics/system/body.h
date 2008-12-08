#ifndef _PHYSICSBODY_H_
#define _PHYSICSBODY_H_

#include "math/mtx4x3.h"
#include "containers/intr_list.h"
#include "physics/collision/system/constraint.h"
#include "physics\collision\shapes\shape.h"

	struct physicssystem;
	struct constraint_t;
	struct shape_t;
	struct shape_desc;

	enum bodystate
	{
		BODYSTATE_DYNAMIC,
		BODYSTATE_STATIC
	};

	struct bodydesc
	{
		mtx4x3	pos;
		vec3		vel;
		vec3		rotvel;

		float			mass;
		mtx3x3	inertia;

		bodystate		is_static;

		bodydesc()
		{
			pos.identity();
			vel.clear();
			rotvel.clear();
			mass=1.0f;
			inertia.identity();
			is_static=BODYSTATE_DYNAMIC;
		}
	};

	class constraint_edge;
	struct contact_edge:intr_list_node_base<contact_edge>
	{
	    body_t* other;
	    contact_t* elem;
	};

	struct body_t
	{
		const mtx4x3& get_pos() const;
		const vec3& get_vel() const;
		const vec3& get_rotvel() const;

		void set_pos(const mtx4x3& i_pos);
		void set_vel(const vec3& i_vel);
		void set_rotvel(const vec3& i_rotvel);

		shape_t* add_shape(const shape_desc& i_desc);
		void release_shape(shape_t* i_shape);


		body_t();
		~body_t();

		uint16 array_index;
		bodystate  is_static;

		intr_list<contact_edge> contacts;
		intr_list<constraint_edge> joints;

        intr_list<shape_t> shapes;
        int group_index;

	};
#endif

