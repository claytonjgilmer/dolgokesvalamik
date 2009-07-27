#include "taskmanager.h"

DEFINE_SINGLETON(taskmanager_t);
//taskmanager_t* taskmanager_t::ptr=NULL;

static unsigned WINAPI poolrun(void* i_param)
{
	taskmanager_t* tm=(taskmanager_t*)i_param;
	tm->run();
	return 0;
}

void taskmanager_t::exit()
{
	interlocked_exchange_32(&exit_event,thread_array.size());

	while (exit_event)
		spin_loop();
}

taskmanager_t::taskmanager_t(const taskmanagerdesc_t* desc)
{
//	if (!ptr)
//		ptr=this;

	exit_event=0;
	incomplete_task_count=0;
	thread_array.resize(desc->m_threadnum);
	for (uint32 n=0; n<desc->m_threadnum; ++n)
	{
		thread_array[n].start(&poolrun,this);
		char str[256]; sprintf(str,"working thread #%d",n+1);
		thread_array[n].set_name(string(str));
	}
}

taskmanager_t::~taskmanager_t()
{
	while (incomplete_task_count)
		spin_loop();

	exit();
	if (ptr==this)
		ptr=NULL;
}


void taskmanager_t::flush()
{
	while (incomplete_task_count)
		spin_loop();

	task_buf.flush();
}

void taskmanager_t::run()
{
	while (!exit_event)
	{
		task_t* t;

		if (t=get_task())
		{
			t->run();
			post_process(t);
		}
		else
		{
			spin_loop();
		}
	}

	_InterlockedDecrement((long*)&exit_event);
}

struct ref_struct_t
{
	long task_left;
};

task_t* taskmanager_t::get_task()
{
	task_t* t=pending_queue.pop();
	return t;
}

void taskmanager_t::post_process(task_t * t)
{
	long *ref_index= &((ref_struct_t*)t->ref_index)->task_left;
	_InterlockedDecrement(ref_index);
	_InterlockedDecrement(&incomplete_task_count);
}

void taskmanager_t::spawn_tasks(task_t *task_array[], int task_count)
{
	ref_struct_t* rsptr=(ref_struct_t*)task_buf.alloc(sizeof(ref_struct_t));
	rsptr->task_left=task_count;
	_InterlockedExchangeAdd((long*)&this->incomplete_task_count,task_count);

	for (int n=0; n<task_count; ++n)
	{
		task_array[n]->ref_index=rsptr;
		pending_queue.push(task_array[n]);
	}


	while (rsptr->task_left)
	{
		task_t* t=get_task();

		if (t)
		{
			t->run();
			post_process(t);
		}
		else
		{
			spin_loop();
		}
	}
}

