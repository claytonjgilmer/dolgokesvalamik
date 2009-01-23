#ifndef _plane_h_
#define _plane_h_

	struct plane_t
	{
		plane_t();
		plane_t(const vec3& i_normal, float i_dist);

		void set(const vec3& i_normal, float i_dist);
		void set(const vec3& i_point, const vec3& i_normal);

		float get_distance(const vec3& i_point) const;

		vec3 normal;
		float dist;
	};

	MLINLINE plane_t::plane_t()
	{
	}

	MLINLINE plane_t::plane_t(const vec3& i_normal, float i_dist)
	{
		normal=i_normal;
		dist=i_dist;
	}

	MLINLINE void plane_t::set(const vec3& i_normal, float i_dist)
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

	MLINLINE float plane_t::get_distance(const vec3& i_point) const
	{
		return i_point.dot(normal)+dist;
	}
#endif//_plane_h_
