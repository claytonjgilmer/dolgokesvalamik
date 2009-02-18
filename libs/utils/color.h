#ifndef _color_h_
#define _color_h_

	struct color_f
	{
		float r,g,b,a;
		color_f(){}
		color_f(float i_r,float i_g,float i_b,float i_a):r(i_r),g(i_g),b(i_b),a(i_a){}
	};

	struct color_r8g8b8a8
	{
		uint8 r;
		uint8 g;
		uint8 b;
		uint8 a;

		color_r8g8b8a8(){}
		color_r8g8b8a8(uint8 i_r, uint8 i_g, uint8 i_b, uint8 i_a):r(i_r),g(i_g),b(i_b),a(i_a){}
		void set(uint8 i_r, uint8 i_g, uint8 i_b, uint8 i_a)
		{
			a=i_a;
			r=i_r;
			g=i_g;
			b=i_b;
		}


	};

#endif//_color_h_