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

	long long timer::freq=freqinit();
	double timer::freqd=(double)freqinit();

	void timer::reset()
	{
		start_tick=get_act_tick();
	}

	void timer::stop()
	{
		stop_tick=get_act_tick();
	}

	unsigned timer::get_tick() const
	{
		return unsigned (stop_tick-start_tick);
	}

	float timer::get_seconds() const
	{
		return float(double(stop_tick-start_tick)/freqd);
	}
