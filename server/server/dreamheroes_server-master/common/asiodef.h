#ifndef ASIO_DEF_HEAD
#define ASIO_DEF_HEAD
#include "utilities.h"
#include "mt_buffer.h"
#include "call_back.h"
#include "message_interface.h"

using boost::asio::ip::tcp;

class tcp_session;

static const int MAX_MESSAGE_LEN = 65000;
static const int QUEUE_MESSAGE_LIMIT_SIZE = MAX_MESSAGE_LEN * 30;



struct net_global
{
	// external
	static boost::asio::io_service* get_io_service();
	static void init_net_service( int thread_count, int proc_interval, compress_strategy* cs_imp, bool need_max_speed, int msg_pool_size );
	static void free_net_service();
	static void stop_net_service();
	static message_t* get_message( message_len size, tcp_session* from );
	static void free_message( message_t* p );
	static long get_asio_thread_alive_count();
	static void write_close_log( const char* txt, ... );
	static void update_net_service();

	enum ban_reason_t
	{
		BAN_REASON_WRONG_CHECK_SUM,
		BAN_REASON_TOO_MANY_AUTH_FAIL,
		BAN_REASON_UNCOMPRESS_FAIL,
		BAN_REASON_HACK_PACKET,
		BAN_REASON_TOO_MANY_PACKET,
		BAN_REASON_TOO_MANY_IDLE_CONNECTION,
		BAN_REASON_SHIFTING_GEAR,
	};
};

//using namespace net_global;

#endif
