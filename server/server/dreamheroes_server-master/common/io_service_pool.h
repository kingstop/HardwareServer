#ifndef IO_SERVICE_POOL_HPP
#define IO_SERVICE_POOL_HPP

#include <boost/asio.hpp>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <set>
#include "asiodef.h"

class tcp_session;
class tcp_server;

class advanced_io_service : public boost::asio::io_service
{
public:
	advanced_io_service();
	void add_session( tcp_session* p );
	void del_session( tcp_session* p );
	void update();
	inline char* get_uncompress_buffer() {
		return uncompress_buffer_;
	}

private:
	std::set<tcp_session*> sessions_;
	boost::mutex mutex_;
	char uncompress_buffer_[MAX_MESSAGE_LEN];
};

/// A pool of io_service objects.
class io_service_pool
  : private boost::noncopyable
{
public:
  /// Construct the io_service pool.
  explicit io_service_pool(std::size_t pool_size);

  /// Run all io_service objects in the pool.
  void run();

  /// Stop all io_service objects in the pool.
  void stop();

  void update();

  /// Get an io_service to use.
  boost::asio::io_service& get_io_service();

private:
  typedef boost::shared_ptr<advanced_io_service> io_service_ptr;
  typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

  /// The pool of io_services.
  std::vector<io_service_ptr> io_services_;

  /// The work that keeps the io_services running.
  std::vector<work_ptr> work_;

  /// The next io_service to use for a connection.
  std::size_t next_io_service_;
  std::vector<boost::thread*> threads_;
};

#endif // IO_SERVICE_POOL_HPP
