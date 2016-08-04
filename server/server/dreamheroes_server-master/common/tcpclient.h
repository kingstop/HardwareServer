#ifndef ASIO_TCPCLIENT_HEAD
#define ASIO_TCPCLIENT_HEAD

#include "asiodef.h"
#include "tcpsession.h"


class tcp_client : public tcp_session
{
public:
	tcp_client( boost::asio::io_service& is );
	virtual ~tcp_client();

public:
	virtual void on_connect();
	virtual void on_connect_failed( boost::system::error_code error ) = 0;

public:
	inline void set_reconnect( bool b ) { m_isreconnect = b; }
	void connect( const char* address, unsigned short port );
	void connect( unsigned long address, unsigned short port );
	bool wait_connect( const char* address, unsigned short port );
	bool wait_connect( unsigned long address, unsigned short port );
	void _connect( std::string address, unsigned short port );
	void handle_connect( const boost::system::error_code& error );
	void handle_read_crypt_key( const boost::system::error_code& error );
	void reconnect();
	virtual void run();
	virtual void run_no_wait();
	virtual void push_message( message_t* msg );

protected:
	volatile bool m_isreconnect;
	unsigned int m_reconnect_time;
	volatile bool m_isinitconncted;

	virtual call_back_mgr* _get_cb_mgr();

private:
	virtual void on_accept( tcp_server* p );
	tcp::endpoint m_endpoint;
	volatile unsigned int m_last_reconnect_time;
	volatile bool m_isconnecting;
	std::queue<message_t*> m_queue_recv_msg[2];
	int m_current_recv_queue;
	void _clear_recv_msg();
	boost::mutex m_msg_mutex;

	boost::condition m_conn_cond;
	boost::mutex m_conn_mutex;
	call_back_mgr m_cb_mgr;
};

#endif
