#ifndef _TASK_THREAD_POOL_HEAD_
#define _TASK_THREAD_POOL_HEAD_

#include "utilities.h"

struct task
{
	virtual ~task() {}
	virtual void execute() = 0;
	virtual void end() = 0;
	virtual int get_thread_index() = 0;
};

class task_thread_pool;

class task_thread
{
public:
	task_thread();
	~task_thread();

public:
	void set_father( int index, task_thread_pool* father );
	void run();
	void push( task* p );
	
private:
	std::queue<task*> m_tasks[2];
	int m_current_queue;
	boost::thread* m_thread;
	boost::mutex m_mutex;
	volatile bool m_exit;
	task_thread_pool* m_father;
	int m_index;
};


class task_thread_pool
{
public:
	task_thread_pool();
	~task_thread_pool();

public:
	void startup( int thread_count );
	void shutdown();
	void push_task( task* p );

private:
	int m_thread_count;
	task_thread* m_threads;
	boost::mutex m_mutex;
};


#endif // _TASK_THREAD_POOL_HEAD_