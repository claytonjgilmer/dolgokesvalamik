#ifndef _vec2i_h_
#define _vec2i_h_

	struct vec2i
	{
	public:
		int	x;
		int	y;

		vec2i()
		{
		}

		vec2i(int i_x, int i_y)
		{
			x=i_x;
			y=i_y;
		}
	};
#endif//_vec2i_h_