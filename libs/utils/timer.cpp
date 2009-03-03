#include "timer.h"
#include <Windows.h>

	long long freqinit()
	{
		LARGE_INTEGER freq;
		::QueryPerformanceFrequency(&freq);
		return freq.QuadPart;
	};

	long long get_act_tick()
	{
		LARGE_INTEGER counter;
		::QueryPerformanceCounter(&counter);
		return counter.QuadPart;
	}

	long long timer_t::freq=freqinit();
	double timer_t::freqd=(double)freqinit();

	void timer_t::reset()
	{
		start_tick=get_act_tick();
	}

	void timer_t::stop()
	{
		stop_tick=get_act_tick();
	}

	unsigned timer_t::get_tick() const
	{
		return unsigned (stop_tick-start_tick);
	}

	f32 timer_t::get_seconds() const
	{
		return f32(double(stop_tick-start_tick)/freqd);
	}
