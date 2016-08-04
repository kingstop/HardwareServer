#ifndef ASIO_TCPSESSION_HEAD
#define ASIO_TCPSESSION_HEAD

#include "asiodef.h"

class tcp_server;

class tcp_session
{
public:
	tcp_session( boost::asio::io_service& is );
	virtual ~tcp_session();

public:
	virtual void on_accept( tcp_server* p );
	virtual void on_close( const boost::system::error_code& error );
	virtual void proc_message( const message_t& msg ) = 0;
	virtual void push_message( message_t* msg );
	virtual void reset();
	virtual void run();

public:
	bool is_valid();
	void set_valid( bool b );
	inline bool is_connected() const { return m_isconnected; }
	inline tcp::socket& socket() { return *m_socket; }
	inline tcp_server* get_father() const { return m_father; }
	inline int get_thread_index() const { return m_thread_index; }
	inline boost::asio::io_service& get_io_service() { return m_io_service; }
	std::string get_remote_address_string() const;
	unsigned int get_remote_address_ui() const;
	unsigned short get_remote_port() const;


	void send_message( const void* data,const unsigned int len );
	void close();
	void close_and_ban();
public:
	void handle_read_some( const boost::system::error_code& error, std::size_t bytes_transferred );
	void handle_write( const boost::system::error_code& error, int size, int block_idx );
	void handle_accept( tcp_server* p );
	void handle_close();

	void _write_message( int sent_size );

protected:
	virtual call_back_mgr* _get_cb_mgr();
	friend struct compress_send_task;
	friend struct optimized_compress_send_task;
	void _send_message( message_t* msg );
	void begin_read_message();
	bool _uncompress_message( char* data );
	message_t* _compress_message( const void* data, message_len len, int t_idx );
	message_t* _make_message( const void* data, message_len len );
	void _clear_recv_msg();
	void _clear_send_msg();
	void _on_close( const boost::system::error_code& error );

	boost::asio::io_service& m_io_service;
	volatile bool m_isconnected;
	tcp_server* m_father;
	tcp::socket* m_socket;
	std::queue<message_t*> m_queue_send_msg;
	char m_recv_buffer[MAX_MESSAGE_LEN*2];
	char buffer_[MAX_MESSAGE_LEN];
	std::size_t m_recive_buffer_pos;
	bool m_iscompress;
	
	volatile boost::uint32_t m_isvalid;
	bool m_isclosing;
	int m_thread_index;
	std::string m_remote_ip_str;
	unsigned int m_remote_ip_ui;
	boost::mutex m_mutex;
	char m_sending_data[MAX_MESSAGE_LEN];
	volatile bool m_is_sending_data;
	volatile long m_not_sent_size;
	unsigned char m_send_crypt_key;
	unsigned char m_recv_crypt_key;

};

#endif
