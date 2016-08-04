#include "tcpserver.h"
#include "tcpsession.h"
#include "task_thread_pool.h"
#include "crypt.h"
#include <stdarg.h>
#include "io_service_pool.h"
#include "message_interface.h"


static volatile boost::uint32_t s_asio_thread_count = 0;
static io_service_pool* s_io_service_pool = NULL;
static bool net_service_stoped = false;
static unsigned int s_io_once_listen_session_count = 40;

void net_global::update_net_service()
{
	if( s_io_service_pool )
		s_io_service_pool->update();
}




boost::asio::io_service* net_global::get_io_service()
{
	return &s_io_service_pool->get_io_service();
}



void net_global::init_net_service( int thread_count, int proc_interval, compress_strategy* cs_imp, bool need_max_speed, int msg_pool_size )
{
	message_interface::messageInit(cs_imp);
	net_service_stoped = false;
	s_io_service_pool = new io_service_pool( thread_count );
	s_io_service_pool->run();
    s_io_once_listen_session_count = proc_interval;
}


message_t* net_global::get_message(message_len size, tcp_session* from )
{
	return message_interface::createMessage(from, size);
}

void net_global::free_message( message_t* p )
{
	message_interface::releaseMessage(p);
}

void net_global::stop_net_service()
{
	if( !net_service_stoped )
	{
		net_service_stoped = true;

		s_io_service_pool->stop();
		delete s_io_service_pool;
		s_io_service_pool = NULL;
	}
}

void net_global::free_net_service()
{
	stop_net_service();
}

long net_global::get_asio_thread_alive_count()
{
	return s_asio_thread_count;
}

void net_global::write_close_log( const char* txt, ... )
{
#ifndef _WIN32
    FILE* fp = fopen( "close_reason.log", "a" );
    if( fp )
    {
        va_list ap;
        char buffer[4096];
        va_start(ap, txt);
        vsnprintf(buffer, 4096, txt, ap);
        va_end(ap);

        unsigned int t = (unsigned int)time( NULL );
        int y, m, d, h, min, s;
        convert_unix_time( t, &y, &m, &d, &h, &min, &s );
        fprintf( fp, "[%d-%d-%d %d:%d:%d] %s\n", y, m, d, h, min, s, buffer );
        fclose( fp );
    }
#endif
}

tcp_server::tcp_server( int id )
	: m_id( id ), m_acceptor( NULL ), m_poolcount( 0 ), m_thread_buffer( NULL ), m_thread_count( 0 ), m_cur_thread_index( 0 ),
	m_ttti_mode( false ), m_accepting_count( 0 ), m_limit_mode( false ), m_connection_count( 0 ), m_fp_connection_log( NULL ), m_security( true ),
	m_current_recv_queue( 0 )
{

	m_ttp = new task_thread_pool;
	m_unix_time = (unsigned int)time( NULL );
	
	m_last_log_connection_time = m_unix_time;
	m_last_clean_idle_ip_time = m_unix_time;
}

tcp_server::~tcp_server()
{

}

bool tcp_server::create( unsigned short port, unsigned int poolcount, int thread_count )
{
	tcp::endpoint ep( tcp::v4(), port );
	if( m_acceptor )
	{
		delete m_acceptor;
		m_acceptor = NULL;
	}
	try
	{
		boost::asio::io_service* is = net_global::get_io_service();
		m_acceptor = new tcp::acceptor( *is );
		m_acceptor->open( ep.protocol() );

		boost::asio::socket_base::reuse_address option( true );
		boost::asio::socket_base::linger optiontemp;
		boost::asio::socket_base::linger option2( true, 0 );
		m_acceptor->get_option( optiontemp );
		m_acceptor->set_option( option2 );
		m_acceptor->get_option( optiontemp );

		m_acceptor->set_option( option );
		m_acceptor->set_option( option2 );
		m_acceptor->bind( ep );
		m_acceptor->listen();
	}
	catch( boost::system::system_error& e )
	{
		// error code == 10048, connot listen the same port at a time.
        printf("handle tcp server error code:[%s] port[%d]\n", e.what(), port);
		return false;
	}

	m_ttp->startup( thread_count );
	if( m_thread_buffer )
		delete[] m_thread_buffer;
	m_thread_buffer = new char[THREAD_BUFFER_SIZE * thread_count];
	m_thread_count = thread_count;

	m_poolcount = poolcount;

	if( m_poolcount == 0 )
	{	m_poolcount = 20; }

	for( unsigned int i = 0; i < m_poolcount; ++i )
	{
		free_session( create_session() );
	}

	return true;
}

void tcp_server::handle_accept( tcp_session* p, const boost::system::error_code& error )
{
	{
		boost::mutex::scoped_lock lock( m_proc_mutex );
		--m_accepting_count;
	}
	if( is_ban_ip( p->get_remote_address_ui() ) )
	{
		p->handle_close();
		free_session( p );
		return;
	}

	if( !error )
	{
		p->get_io_service().post( boost::bind( &tcp_session::handle_accept, p, this ) );
	}
	else
	{
		p->handle_close();
		free_session( p );
	}
}

void tcp_server::free_session( tcp_session* p )
{
	boost::mutex::scoped_lock lock( m_proc_mutex );
	p->reset();
	p->set_valid( false );
	m_sessions.push_back( p );
}

void tcp_server::push_message( message_t* msg )
{
	boost::mutex::scoped_lock lock( m_msg_mutex );
	m_queue_recv_msg[m_current_recv_queue].push( msg );
}

void tcp_server::push_task( task* p )
{
	m_ttp->push_task( p );
}

char* tcp_server::get_thread_buffer( int index )
{
	return m_thread_buffer + index * THREAD_BUFFER_SIZE;
}

int tcp_server::generate_thread_index()
{
	boost::mutex::scoped_lock lock( m_proc_mutex );
	int i = m_cur_thread_index;
	if( ++m_cur_thread_index >= m_thread_count )
		m_cur_thread_index = 0;

	if( i >= m_thread_count )
		return 0;
	else
		return i;
}

bool tcp_server::is_ban_ip( unsigned int addr )
{
	boost::mutex::scoped_lock lock( m_ban_mutex );
	std::map<unsigned int, std::pair<unsigned int, net_global::ban_reason_t> >::iterator it = m_banip.find( addr );
	if( it != m_banip.end() )
	{
		if( it->second.first < m_unix_time )
		{
			m_banip.erase( it );
			return false;
		}
		else
			return true;
	}
	else
		return false;
}

void tcp_server::add_ban_ip( unsigned int addr, unsigned int sec, net_global::ban_reason_t br )
{
	char fn[64] = { 0 };
	sprintf( fn, "connection%d.log", m_id );
	m_fp_connection_log = fopen( fn, "a" );
	if( m_fp_connection_log )
	{
		time_t tnow = m_unix_time;
		tm* ptm = localtime( &tnow );
		in_addr ad;
		ad.s_addr = addr;
		char* ip = inet_ntoa( ad );

		fprintf( m_fp_connection_log, "%04d-%02d-%02d %02d:%02d:%02d|Banned IP:[%s], reason:%d\n",
			ptm->tm_year+1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec,
			ip, br );

		fclose( m_fp_connection_log );
	}

	boost::mutex::scoped_lock lock( m_ban_mutex );
	std::map<unsigned int, std::pair<unsigned int, net_global::ban_reason_t> >::iterator it =  m_banip.find( addr );
	if( it != m_banip.end() )
	{
		if( it->second.first < sec )
		{
			it->second.first = sec;
			it->second.second = br;
		}
	}
	else
		m_banip[addr] = std::make_pair( m_unix_time + sec, br );
}

void tcp_server::add_ban_ip( const std::string& addr, unsigned int sec, net_global::ban_reason_t br )
{
	unsigned int iaddr = boost::asio::ip::address_v4::from_string( addr ).to_ulong();
	add_ban_ip( iaddr, sec, br );
}

void tcp_server::remove_ban_ip( unsigned int addr )
{
	boost::mutex::scoped_lock lock( m_ban_mutex );
	m_banip.erase( addr );
}

void tcp_server::run()
{
	_real_run( true );
}

void tcp_server::run_no_wait()
{
	_real_run( false );
}

void tcp_server::stop()
{
	m_sessions.clear();
	m_acceptor->close();
	m_ttp->shutdown();

	if( m_acceptor )
		delete m_acceptor;

	delete m_ttp;
	if( m_thread_buffer )
		delete[] m_thread_buffer;
}

void tcp_server::_real_run( bool is_wait )
{
	m_unix_time = (unsigned int)time( NULL );
	
	{
		boost::mutex::scoped_lock lock( m_proc_mutex );
		while( m_accepting_count < s_io_once_listen_session_count )
		{
			if( m_sessions.size() == 0 )
				break;
			tcp_session* p = m_sessions.front();
			p->set_valid( true );
			m_acceptor->async_accept( p->socket(), boost::bind( &tcp_server::handle_accept, this, p, boost::asio::placeholders::error ) );
			m_sessions.pop_front();
			++m_accepting_count;
		}
	}
	m_cb_mgr.poll();
	
	int proc_index = 0;
	m_msg_mutex.lock();
	if( m_queue_recv_msg[m_current_recv_queue].empty() )
	{
		m_msg_mutex.unlock();
		if( is_wait )
			cpu_wait();
		return;
	}
	proc_index = m_current_recv_queue;
	m_current_recv_queue = !m_current_recv_queue;
	m_msg_mutex.unlock();

	while( !m_queue_recv_msg[proc_index].empty() )
	{
		message_t* msg = m_queue_recv_msg[proc_index].front();

		if( msg->from->is_valid() && msg->from->is_connected() )
			msg->from->proc_message( *msg );

		net_global::free_message( msg );
		m_queue_recv_msg[proc_index].pop();
	}
}
