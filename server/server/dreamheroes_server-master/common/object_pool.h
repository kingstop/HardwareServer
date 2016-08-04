#ifndef __object_pool_h__
#define __object_pool_h__
#include "boost/pool/object_pool.hpp"
#include "boost/thread/mutex.hpp"
#include "exception.h"
#include "my_log.h"

template<class Class>
class ObjectPool : protected boost::object_pool<Class>
{
public:
    typedef typename boost::object_pool<Class>::size_type size_type;
	explicit ObjectPool(const size_type arg_next_size = 32, const size_type arg_max_size = 0)
	:boost::object_pool<Class>(arg_next_size, arg_max_size)
	{
		_obj_count = 0;
	}

	Class* newObject()
	{
		boost::mutex::scoped_lock lock(_mutex);
		Class* p = boost::object_pool<Class>::malloc();
		if (p)
		{ _obj_count ++;}
		return p;
	}
	void deleteObject(Class* p)
	{
		if (p)
		{
			boost::mutex::scoped_lock lock(_mutex);
			this->destroy( p );
			assert(_obj_count > 0);
			_obj_count --;
		}
	}
	void log(const char* className, unsigned int nextCount, unsigned int maxCount)
	{
		Mylog::log_memory( LOG_INFO, "%s_Pool: OnceCount:[%u], MaxCount:[%u]", className, nextCount, boost::object_pool<Class>::get_max_size());
		Mylog::log_memory( LOG_INFO, "%s_Pool: ChunSize[%u], ClassSize[%u]", className, boost::object_pool<Class>::get_requested_size(),sizeof(Class));
		Mylog::log_memory( LOG_INFO, "%s_Pool: UsedCount:[%u], AllocCount:[%u]", className, _obj_count, boost::object_pool<Class>::get_next_size() - nextCount);
		Mylog::log_memory( LOG_INFO, "%s_Pool: UsedMomry[%u], AllocMemory:[%u]", className, boost::object_pool<Class>::get_requested_size() *_obj_count, boost::object_pool<Class>::list.total_size());
	}
protected:
	unsigned int _obj_count;
	boost::mutex _mutex;
	
};

#endif