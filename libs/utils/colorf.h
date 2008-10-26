#ifndef _colorf_h_
#define _colorf_h_

	struct colorf
	{
		float r,g,b,a;

		colorf(){}
		colorf(float i_r,float i_g,float i_b, float i_a):
		r(i_r),g(i_g),b(i_b),a(i_a){}
	};
#endif//_colorf_h_