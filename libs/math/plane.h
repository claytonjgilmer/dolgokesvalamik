#ifndef _plane_h_
#define _plane_h_

#include "dvec3.h"

	struct plane_t
	{
		plane_t();
		plane_t(const vec3& i_normal, f32 i_dist);
		plane_t(const vec3& i_point, const vec3& i_normal);

		void set(const vec3& i_normal, f32 i_dist);
		void set(const vec3& i_point, const vec3& i_normal);

		f32 get_distance(const vec3& i_point) const;
		int classify_point(const vec3&p, float thickness) const;

		vec3 normal;
		f32 dist;
	};

	MLINLINE plane_t::plane_t()
	{
	}

	MLINLINE plane_t::plane_t(const vec3& i_normal, f32 i_dist)
	{
		normal=i_normal;
		dist=i_dist;
	}

	MLINLINE plane_t::plane_t(const vec3& i_point, const vec3& i_normal)
	{
		normal=i_normal;
		normal.normalize();
		dist=-i_point.dot(normal);
	}

	MLINLINE void plane_t::set(const vec3& i_normal, f32 i_dist)
	{
		normal=i_normal;
		dist=i_dist;
	}

	MLINLINE void plane_t::set(const vec3& i_point, const vec3& i_normal)
	{
		normal=i_normal;
		normal.normalize();
		dist=-i_point.dot(normal);
	}

	MLINLINE f32 plane_t::get_distance(const vec3& i_point) const
	{
		return i_point.dot(normal)+dist;
	}

	MLINLINE int plane_t::classify_point(const vec3&p, float thickness) const
	{
		float dist=get_distance(p);

		if (dist>thickness)
			return 1;
		if (dist<-thickness)
			return -1;
		return 0;
	}




	struct dplane_t
	{
		dplane_t();
		dplane_t(const dvec3& i_normal, double i_dist);

		void set(const dvec3& i_normal, double i_dist);
		void set(const dvec3& i_point, const dvec3& i_normal);

		double get_distance(const dvec3& i_point) const;

		dvec3 normal;
		double dist;
	};

	MLINLINE dplane_t::dplane_t()
	{
	}

	MLINLINE dplane_t::dplane_t(const dvec3& i_normal, double i_dist)
	{
		normal=i_normal;
		dist=i_dist;
	}

	MLINLINE void dplane_t::set(const dvec3& i_normal, double i_dist)
	{
		normal=i_normal;
		dist=i_dist;
	}

	MLINLINE void dplane_t::set(const dvec3& i_point, const dvec3& i_normal)
	{
		normal=i_normal;
		normal.normalize();
		dist=-i_point.dot(normal);
	}

	MLINLINE double dplane_t::get_distance(const dvec3& i_point) const
	{
		return i_point.dot(normal)+dist;
	}

#endif//_plane_h_
