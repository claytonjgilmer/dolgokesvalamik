#include "taskmanager2.h"

unsigned WINAPI poolrun(void* i_param)
{
	taskmanager2_t* tm=(taskmanager2_t*)i_param;
	tm->run();
	return 0;
}

taskmanager2_t::taskmanager2_t(int thread_num)
{
	thread_array.resize(thread_num);
	exit=0;
}



void taskmanager2_t::run()
{
	while (!exit)
	{
		task2_t* t;

		if (t=get_task())
		{
			t->run();
			post_process(t);
		}
		else
		{
			tc_spinloop();
		}
	}
}
