#ifndef _plane_h_
#define _plane_h_

	class plane
	{
	public:
		plane();
		plane(const vec3& i_normal, float i_dist);

		void set(const vec3& i_normal, float i_dist);
		void set(const vec3& i_point, const vec3& i_normal);

		float get_distance(const vec3& i_point) const;

		vec3 normal;
		float dist;
	};

	MLINLINE plane::plane()
	{
	}

	MLINLINE plane::plane(const vec3& i_normal, float i_dist)
	{
		normal=i_normal;
		dist=i_dist;
	}

	MLINLINE void plane::set(const vec3& i_normal, float i_dist)
	{
		normal=i_normal;
		dist=i_dist;
	}

	MLINLINE void plane::set(const vec3& i_point, const vec3& i_normal)
	{
		normal=i_normal;
		dist=-i_point.dot(normal);
	}

	MLINLINE float plane::getdistance(const vec3& i_point) const
	{
		return i_point.dot(normal)+dist;
	}
#endif//_plane_h_