#ifndef _shape_h_
#define _shape_h_

#include "math/aabox.h"

	//////////////////////////////////////////////////////////////////////////
	//			shape tipusok felsorolasa
	//////////////////////////////////////////////////////////////////////////
	enum shape_type
	{
		shape_type_invalid=-1,
		shape_type_sphere,
		shape_type_box,
		shape_type_num
	};



	//////////////////////////////////////////////////////////////////////////
	//		shape alaptipus
	//////////////////////////////////////////////////////////////////////////

	struct broadphaseobject;
	struct shape_t
	{
		aabb bounding;
		body_t* body;
		shape_t* next;
		shape_t* prev;
		shape_type type;
		broadphaseobject* collider;
		shape_t():type(shape_type_invalid){}
	};

	struct shape_desc
	{
		shape_type type;
		shape_desc():type(shape_type_invalid){}
	};


	//////////////////////////////////////////////////////////////////////////
	//		sphere shape
	//////////////////////////////////////////////////////////////////////////
	struct sphere_shape_desc:public shape_desc
	{
		vec3 center;
		float radius;

		sphere_shape_desc()
		{
			type=shape_type_sphere;
		}
	};

	struct sphere_shape:public shape_t
	{
		vec3 center;
		float radius;

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
	};
#endif//_shape_h_
