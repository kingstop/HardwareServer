#ifndef _CALL_BACK_HEAD
#define _CALL_BACK_HEAD

#include "utilities.h"


struct i_call
{
	virtual ~i_call(){}
	virtual void call() const = 0;
};

template<class T>
class call_back : public i_call
{
public:
	call_back( const T& t ) : _t( t ) {}
	virtual void call() const
	{
		_t();
	}

private:
	T _t;
};

class call_back_mgr
{
	bool m_changeflag;
	std::queue<i_call*> m_listcb[2];
	int m_current_queue;
	boost::mutex m_mutex;

	template<class T>
	i_call* create_call_back( const T& t )
	{
		return new call_back<T>( t );
	}

public:
	call_back_mgr();
	void poll();
	void clear();

	template<class _Fun, class _Inst>
	void add_cb( _Fun f, _Inst i )
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_listcb[m_current_queue].push( create_call_back( boost::bind( f, i ) ) );
	}

	template<class _Fun, class _Inst, class _Arg1>
	void add_cb( _Fun f, _Inst i, _Arg1 a1 )
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_listcb[m_current_queue].push( create_call_back( boost::bind( f, i, a1 ) ) );
	}

	template<class _Fun, class _Inst, class _Arg1, class _Arg2>
	void add_cb( const _Fun& f, const _Inst& i, const _Arg1& a1, const _Arg2& a2 )
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_listcb[m_current_queue].push( create_call_back( boost::bind( f, i, a1, a2 ) ) );
	}

	template<class _Fun, class _Inst, class _Arg1, class _Arg2, class _Arg3>
	void add_cb( const _Fun& f, const _Inst& i, const _Arg1& a1, const _Arg2& a2, const _Arg3& a3 )
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_listcb[m_current_queue].push( create_call_back( boost::bind( f, i, a1, a2, a3 ) ) );
	}

	template<class _Fun, class _Inst, class _Arg1, class _Arg2, class _Arg3, class _Arg4>
	void add_cb( const _Fun& f, const _Inst& i, const _Arg1& a1, const _Arg2& a2, const _Arg3& a3, const _Arg4& a4 )
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_listcb[m_current_queue].push( create_call_back( boost::bind( f, i, a1, a2, a3, a4 ) ) );
	}

	template<class _Fun, class _Inst, class _Arg1, class _Arg2, class _Arg3, class _Arg4, class _Arg5>
	void add_cb( const _Fun& f, const _Inst& i, const _Arg1& a1, const _Arg2& a2, const _Arg3& a3, const _Arg4& a4, const _Arg5& a5 )
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_listcb[m_current_queue].push( create_call_back( boost::bind( f, i, a1, a2, a3, a4, a5 ) ) );
	}

	template<class _Fun, class _Inst, class _Arg1, class _Arg2, class _Arg3, class _Arg4, class _Arg5, class _Arg6>
	void add_cb( const _Fun& f, const _Inst& i, const _Arg1& a1, const _Arg2& a2, const _Arg3& a3, const _Arg4& a4, const _Arg5& a5, const _Arg6& a6 )
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_listcb[m_current_queue].push( create_call_back( boost::bind( f, i, a1, a2, a3, a4, a5, a6 ) ) );
	}
};



#endif
