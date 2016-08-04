#ifndef __packet_interface_h__
#define __packet_interface_h__
#include "thread_buffer.h"
/*
  this is use for packet buffer all msg need over loading the interface 
  the 
  virtual PacketUsn& Usn(PacketUsn& buff) interface is 
  used by receving buffer, and read data from the buffer.  and it include a PacketHead
  PacketHead is a base class for all  message 

  virtual PacketSn& Sn(PacketSn& buff) const interface is
  used by sending buffer, and write data to the buffer

  at fan.wu. mail:love19862003@gmail.com
*/

struct PacketSn
{	
    explicit PacketSn(u32 max_len):_ts_buff_max(max_len)
    { 
        _ts_buff = new thread_send_buffer(_ts_buff_max) ;
    }

    ~PacketSn()
    {
        if (_ts_buff)
        { delete _ts_buff;}
    }
    void addBuf(const void* nItem,  size_t  nSize)
    { _ts_buff->write((const char*)nItem, nSize);}

    u32   getLenth() const{return _ts_buff->getLen();}
    char* getData() const{return _ts_buff->getBuff();} 

    PacketSn& operator << (const bool prop)				{ addBuf(&prop, sizeof(bool));				return *this ;}
    PacketSn& operator << (const char prop)				{ addBuf(&prop, sizeof(char));				return *this ;}
    PacketSn& operator << (const unsigned char prop)	{ addBuf(&prop, sizeof(unsigned char));		return *this ;}
    PacketSn& operator << (const short prop)			{ addBuf(&prop, sizeof(short));				return *this ;}
    PacketSn& operator << (const unsigned short prop)	{ addBuf(&prop, sizeof(unsigned short));	return *this ;}
    PacketSn& operator << (const int prop)				{ addBuf(&prop, sizeof(int));				return *this ;}
    PacketSn& operator << (const unsigned int prop)		{ addBuf(&prop, sizeof(unsigned int));		return *this ;}
    PacketSn& operator << (const long prop)				{ addBuf(&prop, sizeof(long));				return *this ;}
    PacketSn& operator << (const unsigned long prop)	{ addBuf(&prop, sizeof(unsigned long));		return *this ;}
    PacketSn& operator << (const u64 prop)				{ addBuf(&prop, sizeof(u64));				return *this ;}
    PacketSn& operator << (const s64 prop)				{ addBuf(&prop, sizeof(s64));				return *this ;}
    PacketSn& operator << (const float prop)			{ addBuf(&prop, sizeof(float));				return *this ;}
    PacketSn& operator << (const double prop)			{ addBuf(&prop, sizeof(double));			return *this ;}


    template <typename T>
    PacketSn& operator << (const T& prop) {prop.Sn(*this); return *this;}

    PacketSn& operator << (const std::string& prop)
    {
        size_t slen = prop.length();
        *this << (unsigned int)slen;
        addBuf(prop.c_str(), slen);
        return *this;
    }

    template<typename T>
    PacketSn& operator << (const std::list<T>& prop)
    {
        size_t slen = prop.size();

        *this << (unsigned int)slen;
        for(typename std::list<T>::const_iterator it=prop.begin();it!=prop.end();++it)
        { *this << *it;}

        return *this;
    }

    template<typename T>
    PacketSn& operator << (const std::deque<T>& prop)
    {
        size_t slen = prop.size();

        *this << (unsigned int)slen;
        for(typename std::deque<T>::const_iterator it=prop.begin();it!=prop.end();++it)
        {
            *this << *it;
        }
        return *this;
    }

    template<typename T>
    PacketSn& operator << (const std::vector<T>& prop)
    {
        size_t slen = prop.size();

        *this << (unsigned int)slen;
        for(typename std::vector<T>::const_iterator it=prop.begin();it!=prop.end();++it)
        {
            *this << *it;
        }
        return *this;
    }

    template<typename T>
    PacketSn& operator << (const std::set<T>& prop)
    {
        size_t slen = prop.size();

        *this << (unsigned int)slen;
        for(typename std::set<T>::const_iterator it=prop.begin();it!=prop.end();++it)
        {
            *this << *it;
        }
        return *this;
    }

    template<typename T1, typename T2>
    PacketSn& operator << (const std::map<T1,T2>& prop)
    {
        size_t slen = prop.size();

        *this << (unsigned int)slen;
        for(typename std::map<T1,T2>::const_iterator it=prop.begin();it!=prop.end();++it)
        {
            *this << it->first << it->second;
        }
        return *this;
    }
private:
    thread_send_buffer * _ts_buff;
    const unsigned int  _ts_buff_max;
};

struct PacketHead
{
    PacketHead()
    {
        ph_number = 0;
        ph_trans_id = 0;
        ph_flag = 0;
    }

    u16  ph_number;             // msg number 
    u32  ph_trans_id;           // msg trans id       
    u32  ph_flag;               // msg flag


    virtual PacketUsn& Usn(PacketUsn& buff) 
    {
        buff >> ph_number >> ph_trans_id >> ph_flag; return buff ;
    }
    virtual PacketSn& Sn(PacketSn& buff) const
    {
        buff << ph_number << ph_trans_id << ph_flag; return buff;
    }
};
struct PacketUsn
{
    explicit PacketUsn( char* _data, const unsigned int len)
    {
        _tr_buff = new thread_recv_buffer(_data, len );
        *this >> _packet_head;
        _tr_buff->resetReadPos();
    }
    ~PacketUsn()
    {
        if (_tr_buff)
        { delete _tr_buff;}
    }
    void getBuf(void* nItem, size_t nSize)
    {
        _tr_buff->read((char*)nItem, nSize);
    }

    u32   getLenth() const{return _tr_buff->getLen();}
    char* getData() const{return _tr_buff->getBuff();} 

    PacketHead& getPacketHead() const {return _packet_head ;}

    PacketUsn& operator >> (bool& prop)				{getBuf(&prop, sizeof(bool));			return *this;}
    PacketUsn& operator >> (char& prop)				{getBuf(&prop, sizeof(char));			return *this;}
    PacketUsn& operator >> (unsigned char& prop)	{getBuf(&prop, sizeof(unsigned char));	return *this;}
    PacketUsn& operator >> (short& prop)			{getBuf(&prop, sizeof(short));			return *this;}
    PacketUsn& operator >> (unsigned short& prop)	{getBuf(&prop, sizeof(unsigned short)); return *this;}
    PacketUsn& operator >> (int& prop)				{getBuf(&prop, sizeof(int));			return *this;}
    PacketUsn& operator >> (unsigned int& prop)		{getBuf(&prop, sizeof(unsigned int));	return *this;}
    PacketUsn& operator >> (long& prop)				{getBuf(&prop, sizeof(long));			return *this;}
    PacketUsn& operator >> (unsigned long& prop)	{getBuf(&prop, sizeof(unsigned long));	return *this;}
    PacketUsn& operator >> (u64& prop)				{getBuf(&prop, sizeof(u64));			return *this;}
    PacketUsn& operator >> (s64& prop)				{getBuf(&prop, sizeof(s64));			return *this;}
    PacketUsn& operator >> (float& prop)			{getBuf(&prop, sizeof(float));			return *this;}
    PacketUsn& operator >> (double& prop)			{getBuf(&prop, sizeof(double));			return *this;}

    template <typename T>
    PacketUsn& operator >> (T& prop) {prop.Usn(*this); return *this;}

    PacketUsn& operator >> (std::string& prop)
    {
        unsigned int slen = 0;
        prop.clear();

        *this >> slen;
        prop = std::string(_tr_buff->getBegin(), slen);
        _tr_buff->addReadLen(slen);
        return *this;
    }

    template<typename T>
    PacketUsn& operator >> (std::list<T>& prop)
    {
        unsigned int slen = 0;
        *this >> slen;

        prop.clear();
        T temp;
        for(size_t i=0;i<slen;++i)
        {
            *this >> temp;
            prop.push_back(temp);
        }
        return *this;
    }

    template<typename T>
    PacketUsn& operator >> (std::deque<T>& prop)
    {
        unsigned int slen= 0;
        *this >> slen;

        prop.clear();
        T temp;
        for(size_t i=0;i<slen;++i)
        {
            *this >> temp;
            prop.push_back(temp);
        }
        return *this;
    }

    template<typename T>
    PacketUsn& operator >> (std::vector<T>& prop)
    {
        unsigned int slen = 0;
        *this >> slen;

        prop.clear();
        T temp;
        for(size_t i=0;i<slen;++i)
        {
            *this >> temp;
            prop.push_back(temp);
        }
        return *this;
    }

    template<typename T>
    PacketUsn& operator >> (std::set<T>& prop)
    {
        unsigned int slen = 0;
        *this >> slen;

        prop.clear();
        T temp;
        for(size_t i=0;i<slen;++i)
        {
            *this >> temp;
            prop.insert(temp);
        }
        return *this;
    }

    template<typename T1, typename T2>
    PacketUsn& operator >> (std::map<T1,T2>& prop)
    {
        unsigned int slen = 0;
        *this >> slen;

        prop.clear();
        T1 temp1;
        T2 temp2;
        for(size_t i=0;i<slen;++i)
        {
            *this >> temp1 >> temp2;
            prop[temp1] = temp2;
        }
        return *this;
    }
private:
    thread_recv_buffer * _tr_buff;
    PacketHead _packet_head;
};
#endif