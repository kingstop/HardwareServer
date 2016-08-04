#ifndef __net_type_h__
#define __net_type_h__
#include "common_type.h"
struct net_info
{
    net_info()
    {
        _id = 0;
        _ip = "10.1.1.115";
        _port = 10104;
        _time_out = 0;
        _max_connect = 2000;
        _thread_count = 0;
        _ipv4 = true;
    }

    u8   _id;
    std::string _ip;
    u16  _port;
    u32  _time_out;
    u32  _max_connect;
    u16  _thread_count;
    bool _ipv4;
};

struct service_config
{
    service_config()
    {
        id = 0 ;
        limit_player = 0 ;
        thread_count = 0;
        speed_ = true;
        msg_pool_size = 0; 
        proc_interval = 0;
    }
    u8      id;
    u32     limit_player;
    u16     thread_count;
    u16     proc_interval;
    bool    speed_;
    u32     msg_pool_size;

};

#endif