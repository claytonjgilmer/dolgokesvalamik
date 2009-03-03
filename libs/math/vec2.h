#ifndef _vec2_h_
#define _vec2_h_

	struct vec2
	{
		f32	x;
		f32	y;

		vec2()
		{
		}

		vec2(f32 i_x, f32 i_y)
		{
			x=i_x;
			y=i_y;
		}

		void set(f32 i_x, f32 i_y)
		{
			x=i_x;
			y=i_y;
		}

		f32 dot(const vec2& i_other) const
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

		vec2 operator*(f32 s) const
		{
			return vec2(x*s,y*s);
		}

		f32 squarelength() const
		{
			return x*x+y*y;
		}

		f32 length() const
		{
			return sqrtf(squarelength());
		}

		void normalize()
		{
			f32 l=length();
			x/=l;
			y/=l;
		}
	};
#endif//_vec2_h_
