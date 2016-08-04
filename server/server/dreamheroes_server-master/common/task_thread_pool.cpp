#include "task_thread_pool.h"


task_thread::task_thread() : m_exit( false ), m_father( NULL ), m_index( 0 ), m_current_queue( 0 )
{
	m_thread = new boost::thread( boost::bind( &task_thread::run, this ) );
}

task_thread::~task_thread()
{
	m_exit = true;
	m_thread->join();
	delete m_thread;
}

void task_thread::set_father( int index, task_thread_pool* father )
{
	m_index = index;
	m_father = father;
}

void task_thread::run()
{
	while( !m_exit )
	{
		int proc_index = 0;
		{
			m_mutex.lock();
			if( m_tasks[m_current_queue].empty() )
			{
				m_mutex.unlock();
#ifdef _WIN32
				Sleep( 1 );
#else
				usleep( 5000 );
#endif
				continue;
			}
			
			proc_index = m_current_queue;
			m_current_queue = !m_current_queue;
			m_mutex.unlock();
		}
		while( !m_tasks[proc_index].empty() )
		{
			task* p = m_tasks[proc_index].front();
			p->execute();
			p->end();
			delete p;
			m_tasks[proc_index].pop();
		}
	}
}

void task_thread::push( task* p )
{
	boost::mutex::scoped_lock lock( m_mutex );
	m_tasks[m_current_queue].push( p );
}

task_thread_pool::task_thread_pool() : m_threads( NULL )
{

}

task_thread_pool::~task_thread_pool()
{

}

void task_thread_pool::startup( int thread_count )
{
	if( m_threads )
		shutdown();
	m_threads = new task_thread[thread_count];
	for( int i = 0; i < thread_count; ++i )
		m_threads[i].set_father( i, this );
	m_thread_count = thread_count;
}

void task_thread_pool::shutdown()
{
	if( m_threads )
	{
		delete[] m_threads;
		m_threads = NULL;
	}
}

void task_thread_pool::push_task( task *p )
{
	if( p->get_thread_index() < m_thread_count && p->get_thread_index() >= 0 )
		m_threads[p->get_thread_index()].push( p );
}
