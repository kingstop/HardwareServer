#ifndef _MT_BUFFER_HEAD
#define _MT_BUFFER_HEAD

#include "utilities.h"

template<unsigned int size>
class mt_buffer
{
public:
	mt_buffer() : m_using( 0 ), m_mutex( NULL ) { m_buff = new char[size]; }
	~mt_buffer() { delete[] m_buff; }

public:
	inline long acquire( boost::mutex* m )
	{
		m_mutex = m;
		if( !m_using )
		{
			m_using = 1;
			return 1;
		}
		else
			return 0;
	}
	inline void release()
	{
		if( m_mutex )
		{
			boost::mutex::scoped_lock lock( *m_mutex );
			m_using = 0;
		}
	}
	inline char* get_buff() const
	{
		return m_buff;
	}
	inline unsigned int get_size() const
	{
		return size;
	}

private:
	volatile boost::uint32_t m_using;
	char* m_buff;
	boost::mutex* m_mutex;
};

template<class T>
class mt_auto_ptr
{
public:
	mt_auto_ptr( T t ) : m_ptr( t ){}
	~mt_auto_ptr() {
		if( m_ptr )
			m_ptr->release();
	}
	mt_auto_ptr<T>& operator = ( T ptr ) { m_ptr = ptr; return *this; }
	T operator->() { return m_ptr; }
	T get() const { return m_ptr; }

private:
	T m_ptr;
};

template<unsigned int count, unsigned int size>
class mt_buffer_mgr
{
public:
	typedef mt_buffer<size> buffer_type;
	inline buffer_type* get_free_buffer()
	{
		while( true )
		{
			{
				boost::mutex::scoped_lock lock( m_mutex );
				for( unsigned int i = 0; i < count; ++i )
					if( m_buffers[i].acquire( &m_mutex ) )
						return &m_buffers[i];
			}
			cpu_wait();
		}
		return NULL;
	}

private:
	boost::mutex m_mutex;
	buffer_type m_buffers[count];
};

#endif