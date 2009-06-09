 #ifndef _shape_h_
#define _shape_h_

#include "math/aabox.h"
#include "containers\intr_list.h"

	//////////////////////////////////////////////////////////////////////////
	//			shape tipusok felsorolasa
	//////////////////////////////////////////////////////////////////////////
	enum shape_type
	{
		shape_type_invalid=-1,
		shape_type_sphere,
		shape_type_box,
		shape_type_convex_mesh,
		shape_type_num
	};



	//////////////////////////////////////////////////////////////////////////
	//		shape alaptipus
	//////////////////////////////////////////////////////////////////////////

	struct broadphaseobject;
	struct body_t;

	struct shape_t:public intr_list_node_base<shape_t>
	{
		aabb_t bounding;
		body_t* body;
		shape_type type;
		broadphaseobject* collider;

		shape_t():type(shape_type_invalid){}

		f32 friction;
		f32 restitution;
		int owner_flag;
		int  collision_mask;
	};

	struct shape_desc
	{
		shape_type type;
		shape_desc():type(shape_type_invalid)
		{
			owner_flag=-1;
			collision_mask=0;
			friction=0;
			restitution=0;
		}

		int owner_flag;
		int collision_mask;
		/*
		fricition
		restitution kulon tablazatban
		*/

		f32 friction;
		f32 restitution;
	};


	//////////////////////////////////////////////////////////////////////////
	//		sphere shape
	//////////////////////////////////////////////////////////////////////////
	struct sphere_shape_desc:public shape_desc
	{
		vec3 center;
		f32 radius;

		sphere_shape_desc()
		{
			type=shape_type_sphere;
		}
	};

	struct sphere_shape:public shape_t
	{
		vec3 get_center() const
		{
			return center;
		}

		void get_extreme_point(vec3& p, const vec3& dir)
		{
			p=center+dir*radius;
		}

		vec3 center;
		f32 radius;

		sphere_shape(const sphere_shape_desc& i_desc):
			center(i_desc.center),
			radius(i_desc.radius)
		{
			this->type=shape_type_sphere;
			vec3 radvec; radvec.set(radius,radius,radius);
			bounding.min=center-radvec;
			bounding.max=center+radvec;
		}
	};


	//////////////////////////////////////////////////////////////////////////
	//		box shape
	//////////////////////////////////////////////////////////////////////////
	struct box_shape_desc:public shape_desc
	{
		mtx4x3 pos;
		vec3 extent;

		box_shape_desc()
		{
			type=shape_type_box;
		}
	};

	struct box_shape:public shape_t
	{
		mtx4x3 pos;
		vec3 extent;

		box_shape(const box_shape_desc& i_desc):
		pos(i_desc.pos),
		extent(i_desc.extent)
		{
			this->type=shape_type_box;


			vec3 newcenter=pos.t;
			mtx3x3 absmtx;

			for (unsigned n=0; n<9;++n)
			{
				absmtx[n]=fabsf(this->pos[n]);
			}

			vec3 newextent;
			absmtx.transform3x3(newextent,this->extent);

			this->bounding.min=newcenter-newextent;
			this->bounding.max=newcenter+newextent;
		}

		void get_extreme_point(vec3& p, const vec3& dir)
		{
			p=	pos.t+	
			extent.x*sign(dot(dir,pos.x))*pos.x+
			extent.y*sign(dot(dir,pos.y))*pos.y+
			extent.z*sign(dot(dir,pos.z))*pos.z;

		}
		void get_extreme_point(vec3& p, const mtx4x3& mtx, const vec3& dir) const
		{
			p=	mtx.t+	
			extent.x*sign(dot(dir,mtx.x))*mtx.x+
			extent.y*sign(dot(dir,mtx.y))*mtx.y+
			extent.z*sign(dot(dir,mtx.z))*mtx.z;

		}

		vec3 get_center() const
		{
			return pos.t;
		}
	};


#endif//_shape_h_
