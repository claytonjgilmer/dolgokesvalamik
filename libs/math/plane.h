#ifndef _plane_h_
#define _plane_h_

namespace math
{
	class plane
	{
	public:
		plane();
		plane(const vector3& i_normal, float i_dist);

		void set(const vector3& i_normal, float i_dist);
		void set(const vector3& i_point, const vector3& i_normal);

		float getdistance(const vector3& i_point) const;

		vector3 normal;
		float dist;
	};

	MLINLINE plane::plane()
	{
	}

	MLINLINE plane::plane(const vector3& i_normal, float i_dist)
	{
		normal=i_normal;
		dist=i_dist;
	}

	MLINLINE void plane::set(const vector3& i_normal, float i_dist)
	{
		normal=i_normal;
		dist=i_dist;
	}

	MLINLINE void plane::set(const vector3& i_point, const vector3& i_normal)
	{
		normal=i_normal;
		dist=-i_point.dot(normal);
	}

	MLINLINE float plane::getdistance(const vector3& i_point) const
	{
		return i_point.dot(normal)+dist;
	}

}
#endif//_plane_h_