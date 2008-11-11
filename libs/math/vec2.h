#ifndef _vec2_h_
#define _vec2_h_

	class vec2
	{
	public:
		float	x;
		float	y;

		vec2()
		{
		}

		vec2(float i_x, float i_y)
		{
			x=i_x;
			y=i_y;
		}

		void set(float i_x, float i_y)
		{
			x=i_x;
			y=i_y;
		}

		float dot(const vec2& i_other) const
		{
			return x*i_other.x+y*i_other.y;
		}

		vec2 operator+(const vec2& v) const
		{
			return vec2(x+v.x,y+v.y);
		}

		vec2 operator-(const vec2& v) const
		{
			return vec2(x-v.x,y-v.y);
		}

		vec2 operator*(float s) const
		{
			return vec2(x*s,y*s);
		}

		float squarelength() const
		{
			return x*x+y*y;
		}

		float length() const
		{
			return sqrtf(squarelength());
		}

		void normalize()
		{
			float l=length();
			x/=l;
			y/=l;
		}
	};
#endif//_vec2_h_
