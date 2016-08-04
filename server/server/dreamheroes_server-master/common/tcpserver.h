#ifndef ASIO_TCPSERVER_HEAD
#define ASIO_TCPSERVER_HEAD


#include "asiodef.h"

class tcp_session;

struct task;
class task_thread_pool;

class tcp_server
{
public:
	tcp_server( int id );
	virtual ~tcp_server();

public:
	bool create( unsigned short port, unsigned int poolcount, int thread_count );
	void handle_accept( tcp_session* p, const boost::system::error_code& error );
	inline int get_id() const { return m_id; }
	void free_session( tcp_session* p );
	void push_message( message_t* msg );

	virtual void run();
	virtual void run_no_wait();
	void push_task( task* p );
	char* get_thread_buffer( int index );
	int generate_thread_index();
	bool is_ban_ip( unsigned int addr );
	void add_ban_ip( unsigned int addr, unsigned int sec, net_global::ban_reason_t br );
	void add_ban_ip( const std::string& addr, unsigned int sec, net_global::ban_reason_t br );
	void remove_ban_ip( unsigned int addr );

	inline int get_task_thread_count() const { return m_thread_count; }
	inline bool is_thread_task_transfer_id_mode() const { return m_ttti_mode; }
	inline void set_thread_task_transfer_id_mode() { m_ttti_mode = true; }
	inline call_back_mgr* get_cb_mgr() { return &m_cb_mgr; }
	inline void set_limit_mode( bool b ) { m_limit_mode = b; }
	inline bool is_limit_mode() const { return m_limit_mode; }
	inline unsigned int get_unix_time() const { return m_unix_time; }
	inline void set_security( bool b ) { m_security = b; }
	inline bool is_security() const { return m_security; }

public:
	virtual void stop();
	virtual tcp_session* create_session() = 0;

public:
	boost::mutex m_proc_mutex;
	std::map<unsigned int, unsigned int> m_idleip;

protected:
	void _real_run( bool is_wait );
	tcp::acceptor* m_acceptor;
	std::list<tcp_session*> m_sessions;
	std::set<tcp_session*> m_accepting_sessions;
	int m_id;
	unsigned int m_poolcount;
	unsigned int m_accepting_count;
	volatile boost::uint32_t m_connection_count;
	std::queue<message_t*> m_queue_recv_msg[2];
	int m_current_recv_queue;
	task_thread_pool* m_ttp;

	enum { THREAD_BUFFER_SIZE = MAX_MESSAGE_LEN + 32 };
	char* m_thread_buffer;
	int m_thread_count;
	int m_cur_thread_index;
	bool m_ttti_mode;
	boost::mutex m_msg_mutex;
	boost::mutex m_ban_mutex;
	call_back_mgr m_cb_mgr;

	bool m_limit_mode;
	volatile unsigned int m_unix_time;
	std::map<unsigned int, std::pair<unsigned int, net_global::ban_reason_t> > m_banip;
	unsigned int m_last_log_connection_time;
	FILE* m_fp_connection_log;
	unsigned int m_last_clean_idle_ip_time;
	bool m_security;
};

#endif
