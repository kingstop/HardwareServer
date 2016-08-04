#include "tcpsession.h"
#include "tcpserver.h"
#include "crypt.h"
#include "task_thread_pool.h"
#include "io_service_pool.h"
#include "message_interface.h"

struct compress_send_task : public task
{
	compress_send_task( const void* src, unsigned short l, tcp_session* s, int ti ) : len( l ), session( s ), thread_index( ti )
	{
		data = (char*)malloc( len );
		memcpy( data, src, len );
	}
	virtual void execute()
	{
		if( session->is_valid() )
			result = session->_compress_message( data, len, get_thread_index() );
	}

	virtual void end()
	{
		if( session->is_valid() && session->is_connected() )
			session->_send_message( result );
	}

	~compress_send_task()
	{
		free( data );
	}

	virtual int get_thread_index()
	{
		return thread_index;
	}

private:
	char* data;
	unsigned short len;
	message_t* result;
	tcp_session* session;
	int thread_index;
};

tcp_session::tcp_session( boost::asio::io_service& is )
: m_socket( NULL ), m_io_service( is ), m_isvalid( false ), m_iscompress( false ), m_thread_index( 0 ), m_isclosing( false ), m_recive_buffer_pos( 0 ),
	m_remote_ip_ui( 0 ), m_not_sent_size( 0 ), m_send_crypt_key( 0 ), m_recv_crypt_key( 0 ), m_is_sending_data( false )
{
	memset(m_sending_data, 0, sizeof(m_sending_data));
	memset(m_recv_buffer, 0, sizeof(m_recv_buffer));
	memset(buffer_, 0, sizeof(buffer_));
}

tcp_session::~tcp_session()
{
	_clear_send_msg();
}

bool tcp_session::is_valid()
{
	return interlocked_read( &m_isvalid ) != 0;
}

void tcp_session::set_valid( bool b )
{
	interlocked_write( &m_isvalid, b ? 1 : 0 );
}

void tcp_session::on_accept( tcp_server* p )
{
}

void tcp_session::handle_accept( tcp_server* p )
{
	set_valid( true );
	m_recive_buffer_pos = 0;
	m_isconnected = true;
	m_father = p;

	//m_send_crypt_key = rand() % 255 + 1;
	//m_recv_crypt_key = ;
	//send_message( &m_send_crypt_key, 1 );
	begin_read_message();
	m_thread_index = p->generate_thread_index();

	m_remote_ip_str = get_remote_address_string();
	m_remote_ip_ui = get_remote_address_ui();

	_get_cb_mgr()->add_cb( &tcp_session::on_accept, this, p );

	static_cast<advanced_io_service&>( m_io_service ).add_session( this );
}

std::string tcp_session::get_remote_address_string() const
{
	if( m_remote_ip_str.length() )
		return m_remote_ip_str;

	try
	{
		return m_socket->remote_endpoint().address().to_string();
	}
	catch( ... )
	{
		return "";
	}
}

unsigned int tcp_session::get_remote_address_ui() const
{
	if( m_remote_ip_ui )
		return m_remote_ip_ui;

	try
	{
		return m_socket->remote_endpoint().address().to_v4().to_ulong();
	}
	catch( ... )
	{
		return 0;
	}
}

unsigned short tcp_session::get_remote_port() const
{
	try
	{
		return m_socket->remote_endpoint().port();
	}
	catch( ... )
	{
		return 0;
	}
}

void tcp_session::send_message( const void* data, const unsigned int len )
{
	if( !is_valid() || !m_isconnected || !data || !len || len > MAX_MESSAGE_LEN - 1 /*|| m_send_crypt_key == 0*/ )
		return;

	if( m_father )
	{
		if( m_iscompress )
		{
			int ti = 0;
			if( m_father->is_thread_task_transfer_id_mode() )
			{
				unsigned int transferid = *(unsigned int*)( (const char*)data + 4 );
				ti = transferid % m_father->get_task_thread_count();
			}
			else
				ti = get_thread_index();

			task* t = NULL;
			t = new compress_send_task( data, len, this, ti );
			m_father->push_task( t );
		}
		else
			_send_message( _make_message( data, len ) );
	}
	else
	{	_send_message(_compress_message( data, len, -1 ) );}
}

void tcp_session::close()
{
	m_io_service.post( boost::bind( &tcp_session::handle_close, this ) );
}

void tcp_session::close_and_ban()
{
	if( m_father )
		m_father->add_ban_ip( this->get_remote_address_ui(), 5, net_global::BAN_REASON_HACK_PACKET );
	this->close();
	net_global::write_close_log( "IP:[%s] recv hack packet. ban for 5 seconds", this->get_remote_address_string().c_str() );
}

void tcp_session::handle_close()
{
	if (m_socket != NULL)
	{
		boost::system::error_code ignored_ec;
		m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

		try
		{
			m_socket->close();
		}
		catch (const boost::system::error_code&)
		{

		}
	}

	//m_send_crypt_key = 0;
	//m_recv_crypt_key = 0;


	m_isconnected = false;
	m_thread_index = 0;
}

void tcp_session::run()
{
}

call_back_mgr* tcp_session::_get_cb_mgr()
{
	if( m_father )
		return m_father->get_cb_mgr();
	else
	{
		//assert( 0 );
		return NULL;
	}
}

void tcp_session::_send_message( message_t* msg )
{
	if( !is_connected() )
		return;

	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_queue_send_msg.push( msg );
		m_not_sent_size += msg->len;
		if( m_father && m_father->is_limit_mode() )
		{
			if( m_not_sent_size >= QUEUE_MESSAGE_LIMIT_SIZE )
			{
				close();
				//net_global::write_close_log( "IP:[%s] close, message queue is full", this->get_remote_address_string().c_str() );
				return;
			}
		}
	}

	m_io_service.post( boost::bind( &tcp_session::_write_message, this, 0 ) );
}

void tcp_session::begin_read_message()
{
	m_recive_buffer_pos = 0;
	m_socket->async_read_some(boost::asio::buffer(buffer_, sizeof(buffer_)),
		boost::bind(&tcp_session::handle_read_some, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

bool tcp_session::_uncompress_message( char* data )
{
	message_len size = MAX_MESSAGE_LEN;
	message_t* msg  = message_interface::uncompress(this, data, &m_recv_crypt_key, static_cast<advanced_io_service&>( m_io_service ).get_uncompress_buffer(), size );
	if (NULL == msg)
	{
		if( m_father )
		{
			m_father->add_ban_ip( this->get_remote_address_ui(), 120, net_global::BAN_REASON_WRONG_CHECK_SUM );
		}
		this->close();
		return false;
	}
	
	push_message( msg );
	return true;
}

static char static_compress_buffer[MAX_MESSAGE_LEN];

message_t* tcp_session::_compress_message( const void* data, message_len len, int t_idx )
{
	message_len size = MAX_MESSAGE_LEN;
	char* buffptr = NULL;
	if( m_father && t_idx >= 0 )
		buffptr = m_father->get_thread_buffer( t_idx );
	else
		buffptr = static_compress_buffer;

	return message_interface::compress(this, (const char*)data, len, buffptr, size ,&m_send_crypt_key);
}

message_t* tcp_session::_make_message( const void* data, message_len len )
{
	return message_interface::makeMessage(this, (const char*)data, len, &m_send_crypt_key, false);
}

void tcp_session::_clear_send_msg()
{
	boost::mutex::scoped_lock lock( m_mutex );
	while( !m_queue_send_msg.empty() )
	{
		net_global::free_message( m_queue_send_msg.front() );
		m_queue_send_msg.pop();
	}
}

void tcp_session::_on_close( const boost::system::error_code& error )
{
	//m_send_crypt_key = 0;
	//m_recv_crypt_key = 0;
	_clear_send_msg();

	boost::system::error_code ignored_ec;
	m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

	try
	{
		m_socket->close();
	}
	catch( const boost::system::error_code& )
	{
	}
	m_isconnected = false;

	if( _get_cb_mgr() )
	{
		_get_cb_mgr()->add_cb( &tcp_session::on_close, this, error );
	}
}

void tcp_session::_write_message( int sent_size )
{
	boost::mutex::scoped_lock lock( m_mutex );
	m_not_sent_size -= sent_size;

	if( !is_connected() )
		return;

	// optimize for sending-data frequently
	if( m_is_sending_data )
		return;

	if( m_queue_send_msg.empty() )
		return;

	int len = 0;
	while( !m_queue_send_msg.empty() )
	{
		message_t* msg = m_queue_send_msg.front();
		if( len + msg->len <= MAX_MESSAGE_LEN )
		{
			memcpy( m_sending_data + len, msg->data, msg->len );
			len += msg->len;
			m_queue_send_msg.pop();
			net_global::free_message( msg );
		}
		else
			break;
	}

	boost::asio::async_write( *m_socket,
		boost::asio::buffer( m_sending_data,
		len ),
		boost::bind( &tcp_session::handle_write, this,
		boost::asio::placeholders::error, len, 0 ) );
	
	m_is_sending_data = true;
}

void tcp_session::handle_read_some( const boost::system::error_code& error, std::size_t bytes_transferred )
{
	if( !is_valid() )
		return;

	if( !error )
	{
		std::size_t now_pos = 0;
		memcpy( m_recv_buffer + m_recive_buffer_pos, buffer_, bytes_transferred );
		m_recive_buffer_pos += bytes_transferred;

		if( m_recive_buffer_pos >= MESSAGE_HEAD_LEN )
		{
			message_len len = *(message_len *)m_recv_buffer;
			if( len < MESSAGE_HEAD_LEN || len > MAX_MESSAGE_LEN )
			{
				close_and_ban();
				return;
			}
			while( m_recive_buffer_pos - now_pos >= len )
			{
				if( !_uncompress_message( m_recv_buffer + now_pos ) )
				{
					_on_close( error );
					return;
				}
				now_pos += len;
				if( m_recive_buffer_pos - now_pos >= MESSAGE_HEAD_LEN )
				{
					len = *(message_len*)( m_recv_buffer + now_pos );

					if( len < MESSAGE_HEAD_LEN || len > MAX_MESSAGE_LEN )
					{
						close_and_ban();
						return;
					}
				}
				else
					break;
			}
			if( now_pos > 0 && m_recive_buffer_pos > now_pos )
				memmove( m_recv_buffer, m_recv_buffer + now_pos, m_recive_buffer_pos - now_pos );
			m_recive_buffer_pos -= now_pos;
		}
		m_socket->async_read_some(boost::asio::buffer(buffer_, sizeof(buffer_)),
			boost::bind(&tcp_session::handle_read_some, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	else
		_on_close( error );
}

void tcp_session::handle_write( const boost::system::error_code& error, int size, int block_idx )
{
	if( !is_valid() )
		return;

	if( !error )
	{
		{
			boost::mutex::scoped_lock lock( m_mutex );
			m_is_sending_data = false;
		}
		_write_message( size );
	}
	else
		_on_close( error );
}

void tcp_session::on_close( const boost::system::error_code& error )
{
	if( m_father )
	{
		m_father->free_session( this );
		static_cast<advanced_io_service&>( m_io_service ).del_session( this );
	}
}

void tcp_session::push_message( message_t* msg )
{
	if( m_father )
		m_father->push_message( msg );
}

void tcp_session::reset()
{
	m_recive_buffer_pos = 0;
	m_isconnected = false;
	m_father = NULL;
	m_isvalid = false;
	m_thread_index = 0;
	m_is_sending_data = false;
	m_isclosing = false;
	m_remote_ip_str.clear();
	m_remote_ip_ui = 0;

	m_not_sent_size = 0;
	//m_send_crypt_key = 0;
	//m_recv_crypt_key = 0;
	_clear_send_msg();
	if( m_socket )
		delete m_socket;
	m_socket = new tcp::socket( m_io_service );
}

