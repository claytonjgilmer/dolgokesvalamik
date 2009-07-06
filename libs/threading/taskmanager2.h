#ifndef _taskmanager2_h_
#define _taskmanager2_h_

#define task_pool_size 65536
#include "utils/scratchpad.h"
#include "thread.h"
#include "containers/lockfreequeue.h"
#include "containers/vector.h"

struct task2_t
{
	task2_t()
	{
//		nextsk=0;
	}

	virtual void run() =0;
	void* operator new(size_t i_size);

	void* ref_index;
	task2_t* next;
};



struct taskmanager2_t
{
	taskmanager2_t(int thread_num);
	~taskmanager2_t();

	void spawn_tasks(task2_t* task_array[], int task_count);
	void flush();
	void exit();

	static taskmanager2_t* ptr;

	void run();
	task2_t* get_task();
	void post_process(task2_t*);

	void* alloc_task(uint32 size)
	{
		return task_buf.alloc(size);
	}

	int exit_event;
	long incomplete_task_count;


	lockfree_queue_t<task2_t> pending_queue;
	vector<thread> thread_array;

	scratch_pad_t<task_pool_size> task_buf;
};

MLINLINE void* task2_t::operator new(size_t size)
{
	return taskmanager2_t::ptr->alloc_task(size);
}

#endif//_taskmanager2_h_