#ifndef _timer_h_
#define _timer_h_

#include "math/math.h"

	struct timer_t
	{
		void reset();
		void stop();
		unsigned get_tick() const;
		f32 get_seconds() const;



		long long start_tick;
		long long stop_tick;

		static long long freq;
		static double freqd;
	};
#endif//_timer_h_
