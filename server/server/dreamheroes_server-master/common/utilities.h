#ifndef _NEW_COMMON_UTILITIES_HEAD
#define _NEW_COMMON_UTILITIES_HEAD
#ifdef _WIN32
#ifndef BOOST_ALL_NO_LIB
#define BOOST_ALL_NO_LIB
#endif
#endif

#ifndef BOOST_HAS_THREADS
#define BOOST_HAS_THREADS
#endif
#include <algorithm>
#include <cstdlib>
#include <set>
#include <queue>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/system/system_error.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>
#ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#else
#include <process.h>
#endif
#include <time.h>
#include <stdio.h>
#include <signal.h>

#include <stdio.h>
void signal_handle(int n);
bool is_in_today(const time_t * t);
bool is_in_current_mon(const time_t * t);
void save_pid();
template<class T>
void safe_delete( T*& p )
{
	if( p )
	{
		delete p;
		p = NULL;
	}
}

template<class T>
void safe_release( T*& p )
{
	if( p )
	{
		p->release();
		p = NULL;
	}
}

template<class T>
class auto_array_ptr
{
public:
	auto_array_ptr( T* p ) : m_ptr( p ){}
	~auto_array_ptr()
	{
		delete[] m_ptr;
	}
private:
	T* m_ptr;
};

template<class T>
class smart_ptr
{
public:
	typedef smart_ptr<T> self_type;
	smart_ptr( T* p ) : m_ptr( p ), m_owner( true )
	{
	}

	smart_ptr( const self_type& sp ) : m_owner( true ), m_ptr( sp.m_ptr )
	{
		sp.m_owner = false;
	}

	~smart_ptr()
	{
		if( m_ptr && m_owner )
			delete m_ptr;
	}

	smart_ptr& operator = ( T* p )
	{
		m_ptr = p;
		m_owner = true;
		return *this;
	}

	smart_ptr& operator = ( const self_type& sp )
	{
		m_ptr = sp.m_ptr;
		sp.m_owner = false;
		m_owner = true;
	}

	const T* operator -> () const
	{
		return (const T*)m_ptr;
	}

	T* operator -> ()
	{
		return (T*)m_ptr;
	}

	const T& operator * () const
	{
		return *m_ptr;
	}

	T& operator *()
	{
		return *m_ptr;
	}

	operator T* ()
	{
		return m_ptr;
	}

	operator const T* () const
	{
		return m_ptr;
	}

	operator bool () const
	{
		return m_ptr != NULL;
	}
private:
	mutable bool m_owner;
	T* m_ptr;
};

boost::uint32_t interlocked_increment( volatile boost::uint32_t* target );
boost::uint32_t interlocked_decrement( volatile boost::uint32_t* target );
void interlocked_write( volatile boost::uint32_t* target, boost::uint32_t value );
boost::uint32_t interlocked_read( volatile boost::uint32_t* target );
boost::uint32_t interlocked_compare_exchange( volatile boost::uint32_t* target, boost::uint32_t value, boost::uint32_t comp );
void cpu_wait();
unsigned int generate_table_key();
void encrypt_string( const std::string& in, std::string& out, unsigned int table );
void decrypt_string( const std::string& in, std::string& out, unsigned int table );
unsigned int get_tick_count();
#ifndef _WIN32
const char* itoa( int value, char* str, int base );
#endif

template<class T>
bool combine_checksum( std::pair<int, T>* a, int n, int m, int b[], const int M, const int sum, std::vector<std::pair<int, T>*>& out )
{
	for(int i = n; i >= m; --i )
	{
		if( m <= 0 )
			continue;
		b[m-1] = i - 1;
		if( m > 1 )
		{
			if( combine_checksum( a, i - 1, m - 1, b, M, sum, out ) )
				return true;
		}
		else
		{
			int s = 0;
			bool su = false;
			for( int j = M - 1; j >= 0; --j )
			{
				s += a[b[j]].first;
				if( s == sum )
					su = true;
			}

			if( su )
			{
				int s = 0;
				out.clear();
				for( int j = M - 1; j >= 0; --j )
				{
					s += a[b[j]].first;
					out.push_back( &a[b[j]] );
					if( s == sum )
						return true;
				}
			}
		}
	}
	return false;
}

template<class T>
bool search_combine_checksum( T* a, int length, int sum, std::vector<T*>& out )
{
	int b[1024] = { 0 };
	if( length > 1024 )
		return false;

	for( int j = 1; j <= length; ++j )
		if( combine_checksum( a, length, j, b, j, sum, out ) )
			return true;
	return false;
}

class function_spend_time_monitor
{
public:
	function_spend_time_monitor( const char* fn_name );
	~function_spend_time_monitor();



private:

	unsigned int m_begin;

	unsigned int m_end;

	std::string m_name;

};



void convert_unix_time( unsigned int t, int* outyear, int* outmonth, int* outday, int* outhour, int* outminute, int* outsecond );
void build_unix_time_to_string( unsigned int t, std::string& out );
unsigned int make_unix_time( int year, int month, int day, int hour, int minute, int second );
bool same_month( unsigned int t1, unsigned int t2 );
bool same_week( unsigned int t1, unsigned int t2 );
bool is_dayofweek( unsigned int t, unsigned char dow );
bool in_duration( unsigned int t, unsigned char hstart, unsigned char hend );
int SplitStringA(const std::string& strIn, const std::string& strDelimiter, std::vector<std::string>& ret);
bool isIntger(std::string str);
bool is_valid_string( const std::string& str );
char* get_and_create_static_buffer( std::size_t size );
std::string get_time(time_t cur_time);

struct memory_leak_monitor
{
	virtual ~memory_leak_monitor() {}
	virtual void init() = 0;
	static memory_leak_monitor* s_instance;
};

class data_stream_verifier
{
public:
	data_stream_verifier();
	void reset();
	unsigned int make_checksum( const void* data, unsigned int length );
	bool verify_data_stream( const void* data, unsigned int length, unsigned int checksum );
private:
	unsigned int get_magic_number( unsigned int key );
	unsigned int send_index;
	unsigned int recv_index;
};

#endif // _NEW_COMMON_UTILITIES_HEAD

