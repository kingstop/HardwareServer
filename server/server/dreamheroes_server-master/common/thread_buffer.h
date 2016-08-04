#ifndef __thread_buffer_h__
#define __thread_buffer_h__
#include "common_header.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include "pthread.h"
#endif
#include "boost/thread/mutex.hpp"
#include "exception.h"



struct thread_packet_buffer
{
    enum
    {
        _packet_send_buffer_,
        _packet_recv_buffer_,
        _packet_compress_buffer_,
        _packet_buffer_max_,

        _buffer_default_length = 65000,
    };

    thread_packet_buffer(unsigned int id) :_thread_id(id)
    {
        for (unsigned int  i = 0; i < _packet_buffer_max_; ++i)
        {
            _buffer_array[i] = NULL;
            _buffer_length[i] = 0;
        }
    }

    ~thread_packet_buffer()
    {
        for (unsigned int  i = 0; i < _packet_buffer_max_; ++i)
        {
            if (_buffer_array[i])
            {   free(_buffer_array[i]);}
            _buffer_length[i] = 0;
        }
    }
    char* getThreadBuffer(unsigned int ntype, unsigned int& maxlen)
    {
        if (ntype < _packet_buffer_max_)
        {
            if (maxlen > _buffer_length[ntype])
            {
                if (_buffer_array[ntype])
                {   free(_buffer_array[ntype]);}
                
                if (maxlen < _buffer_default_length)
                {
                    _buffer_length[ntype] =_buffer_default_length;
                }else
                {
                    _buffer_length[ntype] = maxlen * 150 / 100; // 1.5 ±¶µÝÔö
                }
                
                _buffer_array[ntype] = (char*)malloc(_buffer_length[ntype]);
                memset(_buffer_array[ntype], 0 , _buffer_length[ntype]);
            }

            maxlen =  _buffer_length[ntype];
            return _buffer_array[ntype];
        }else
        {
            THROW_EXCEPTION(BuffException("buffer ntupe is error"));
            maxlen = 0;
            return NULL;
        }
    }

    const unsigned int _thread_id;
    char* _buffer_array[_packet_buffer_max_];
    unsigned int _buffer_length[_packet_buffer_max_];
};



struct thread_buffer_manager
{
    static boost::mutex _thread_mutex;
    static std::vector<thread_packet_buffer*> thread_buffer_vector;

    static void releaseThreadBuffer()
    {
#ifdef _WIN32
        unsigned int thread_id = (unsigned int )GetCurrentThreadId();
#else
        unsigned int thread_id = pthread_self();
#endif
        boost::mutex::scoped_lock lock(_thread_mutex);
        std::vector<thread_packet_buffer*>::iterator it = thread_buffer_vector.begin();
        for (it; it != thread_buffer_vector.end(); ++it)
        {
            thread_packet_buffer* p = *it;
            if (thread_id = p->_thread_id)
            {
                thread_buffer_vector.erase(it);
                delete p;
                return ;
            }
        }
    }

    static char* getThreadSendBuffer(unsigned int& maxlen)
    {
        return getThreadBuffer(thread_packet_buffer::_packet_send_buffer_, maxlen);
    }
    static char* getThreadRecvBuffer(unsigned int& maxlen)
    {
        return getThreadBuffer(thread_packet_buffer::_packet_recv_buffer_, maxlen);
    }
    static char* getThreadCompressBuffer(unsigned int& maxlen)
    {
        return getThreadBuffer(thread_packet_buffer::_packet_compress_buffer_, maxlen);
    }

private:
    static char* getThreadBuffer(unsigned int ntype, unsigned int& maxlen)
    {
#ifdef _WIN32
        unsigned int thread_id = (unsigned int )GetCurrentThreadId();
#else
        unsigned int thread_id = pthread_self();
#endif
        thread_packet_buffer* p = NULL;
        std::vector<thread_packet_buffer*>::iterator it = thread_buffer_vector.begin();
        for (it; it != thread_buffer_vector.end(); ++it)
        {
            thread_packet_buffer* pit = *it;
            if (thread_id = pit->_thread_id)
            {
               p = pit;
               break;
            }
        }

        if (NULL == p)
        {
            p = new thread_packet_buffer(thread_id);
            boost::mutex::scoped_lock lock(_thread_mutex);
            thread_buffer_vector.push_back(p);
        }
        return p->getThreadBuffer(ntype, maxlen);
    }
};

struct thread_send_buffer
{
    explicit thread_send_buffer( const unsigned int maxlen) : _max_length(maxlen), _buff_pos(0)
    {
        _buff = thread_buffer_manager::getThreadSendBuffer(_max_length);
    }
    bool write( const char* nItem, unsigned int len)
    {
        if (_buff_pos + len >= _max_length)
        {	
            THROW_EXCEPTION(BuffException("send_buff out of rang"));
            return false;
        }else
        {
            memcpy(_buff + _buff_pos, nItem, len);
            _buff_pos += len;
            return true;
        }
    }

    char* getBuff()const {return _buff;}
    unsigned int getLen() const {return _buff_pos;}
    void addWriteLen(unsigned int add){ _buff_pos += add; assert(_buff_pos < _max_length) ;}

    unsigned int getLastLen() const {return _max_length - _buff_pos ;}
    char* getBegin() const {return _buff + _buff_pos ;}
private:
    char* _buff;
    unsigned int _buff_pos;
    unsigned int _max_length;
};
struct thread_recv_buffer
{
    explicit thread_recv_buffer(const char* data, unsigned int len):_buff_len(len), _buff_pos(0), _max_length(len + 1)
    {
     	_buff = thread_buffer_manager::getThreadRecvBuffer(_max_length);
     	_buff_pos = 0;
     	memcpy(_buff, data, len);
    }
     
    char* getBuff()const {return _buff;}
    unsigned int getLen() const {return _buff_len;}

    char* getBegin() const {return _buff + _buff_pos ;}
    void addReadLen(unsigned int add){_buff_pos += add; assert(_buff_pos <= _buff_len);}
    unsigned int getLastLen() const {return _buff_len - _buff_pos ;}
     
    bool read(char* nItem, unsigned int len)
    {
     	if (_buff_pos + len > _buff_len)
     	{
     		THROW_EXCEPTION(BuffException("recv_buff out of rang"));
     		return false;
     	}else
     	{
     		memcpy(nItem, _buff + _buff_pos, len);
     		_buff_pos += len;
     
     		if(_buff_pos  == _buff_len)
     		{ resetReadPos();}
     		return true;
     	}
    }
     	
    void resetReadPos()
    {
     	_buff_pos = 0;
    }
private:
    char* _buff;
    unsigned int _buff_pos;
    unsigned int _max_length;
    const unsigned int _buff_len;
};
#endif