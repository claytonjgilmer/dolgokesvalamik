#ifndef _taskmanager2_h_
#define _taskmanager2_h_

struct taskmanager2_t
{
	taskmanager2_t();
	~taskmanager2_t();
	void spawn_tasks(task_t* task_array[], int task_count);

	vector<thread> thread_array;
};

MLINLINE void taskmanager2_t::spawn_tasks(task_t *task_array[], int task_count)
{
}
#endif//_taskmanager2_h_