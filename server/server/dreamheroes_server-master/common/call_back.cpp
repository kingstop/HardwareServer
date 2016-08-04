#include "call_back.h"

call_back_mgr::call_back_mgr() : m_current_queue( 0 ) {

}

void call_back_mgr::poll()
{
	int proc_index = 0;
	{
		boost::mutex::scoped_lock lock( m_mutex );
		if( m_listcb[m_current_queue].empty() )
			return;
		
		proc_index = m_current_queue;
		m_current_queue = !m_current_queue;
	}
	while( !m_listcb[proc_index].empty() )
	{
		i_call* p = m_listcb[proc_index].front();
		p->call();
		delete p;
		m_listcb[proc_index].pop();
	}
}

void call_back_mgr::clear()
{
	boost::mutex::scoped_lock lock( m_mutex );
	for( int i = 0; i < 2; ++i )
	{
		while( m_listcb[i].empty() )
		{
			delete m_listcb[i].front();
			m_listcb[i].pop();
		}
	}
}
