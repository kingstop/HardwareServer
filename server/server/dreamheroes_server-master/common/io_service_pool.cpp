#include "io_service_pool.h"
#include <stdexcept>
#include <boost/bind.hpp>
#include "tcpsession.h"

advanced_io_service::advanced_io_service()
{
}

void advanced_io_service::add_session( tcp_session* p )
{
	boost::mutex::scoped_lock lock( mutex_ );
	sessions_.insert( p );
}

void advanced_io_service::del_session( tcp_session* p )
{
	boost::mutex::scoped_lock lock( mutex_ );
	sessions_.erase( p );
}

void advanced_io_service::update()
{
	std::vector<tcp_session*> tmp;
	{
		boost::mutex::scoped_lock lock( mutex_ );
		for( std::set<tcp_session*>::iterator it = sessions_.begin(); it != sessions_.end(); ++it )
			tmp.push_back( *it );
	}

	for( std::vector<tcp_session*>::iterator it = tmp.begin(); it != tmp.end(); ++it )
		(*it)->run();
}

io_service_pool::io_service_pool(std::size_t pool_size)
  : next_io_service_(0)
{
  if (pool_size == 0)
    throw std::runtime_error("io_service_pool size is 0");

  // Give all the io_services work to do so that their run() functions will not
  // exit until they are explicitly stopped.
  for (std::size_t i = 0; i < pool_size; ++i)
  {
    io_service_ptr io_service(new advanced_io_service);
    work_ptr work(new boost::asio::io_service::work(*io_service));
    io_services_.push_back(io_service);
    work_.push_back(work);
  }
}

void io_service_pool::run()
{
  // Create a pool of threads to run all of the io_services.
  for (std::size_t i = 0; i < io_services_.size(); ++i)
  {
	  boost::thread* thread = new boost::thread(
          boost::bind(&boost::asio::io_service::run, io_services_[i]));
    threads_.push_back(thread);
  }
}

void io_service_pool::stop()
{
  // Explicitly stop all io_services.
  for (std::size_t i = 0; i < io_services_.size(); ++i)
    io_services_[i]->stop();

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads_.size(); ++i)
  {
	  threads_[i]->join();
	  delete threads_[i];
  }
  threads_.clear();
}

boost::asio::io_service& io_service_pool::get_io_service()
{
  // Use a round-robin scheme to choose the next io_service to use.
  boost::asio::io_service& io_service = *io_services_[next_io_service_];
  ++next_io_service_;
  if (next_io_service_ == io_services_.size())
    next_io_service_ = 0;
  return io_service;
}

std::size_t next_update_index = 0;

void io_service_pool::update()
{
	io_services_[next_update_index]->update();
	if( ++next_update_index >= io_services_.size() )
		next_update_index = 0;
}

