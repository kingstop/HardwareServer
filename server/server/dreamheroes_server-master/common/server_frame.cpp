#include "server_frame.h"
#include "json_help.h"
#include "utilities.h"
#ifndef WIN32
#include <netdb.h>
#endif

#include <boost/date_time/posix_time/posix_time.hpp>  
bool g_wait_stop = false;
u64 g_server_time = 0;
u64 g_server_start_time = 0;
bool ServerFrame::loadNetConfig(net_info& _config, const char* strfile)
{
	_config._ip = "127.0.0.1";
	_config._port = 22000;
	_config._time_out = 3000;
	_config._max_connect = 2000;
	_config._thread_count = 1;

	JsonHelper help(strfile);
	if (!help.loadJson())
	{   return false;}
	const Json::Value& Root = help.root();
	LAZY_JSON_GET_STRING(_config._ip,				"ip",				Root);
	LAZY_JSON_GET_UINT(_config._port,				"port",				Root);
	LAZY_JSON_GET_UINT(_config._time_out,			"time_out",			Root);
	LAZY_JSON_GET_UINT(_config._max_connect	,		"max_connect",		Root);
	LAZY_JSON_GET_UINT(_config._thread_count,		"thread_count",		Root);

	getIpByname(_config._ip.c_str(), _config._ip);
	return true ;
}
bool ServerFrame::init()
{
	_stop = false;
	_wait_stop = false;
	_service_stop_state = 0;
    _print = false;
	signal(SIGINT, signal_handle);
#ifndef WIN32
	signal(SIGHUP, signal_handle); //* 下面设置三个信号的处理方法
	signal(SIGQUIT, signal_handle);
	save_pid();
#endif
	g_server_start_time = time(NULL);
	return true;
}

void ServerFrame::run()
{

	boost::posix_time::millisec_posix_time_system_config::time_duration_type time_elapse;
	boost::posix_time::ptime nLastTime = boost::posix_time::microsec_clock::universal_time();  
	boost::posix_time::ptime nStartTime = nLastTime ;
	while(!_stop)
	{
		nStartTime = boost::posix_time::microsec_clock::universal_time();
		time_elapse = nStartTime - nLastTime ;
		nLastTime = nStartTime ;
		g_server_time = time(NULL);
		runOnce((u32)(time_elapse.total_milliseconds()));
		onKey();
		if (_wait_stop)
		{ 
			checkStop();
		}
		signalStop();
#ifdef _WIN32
		Sleep(1);
#else
		usleep(1 * 1000);
#endif
	}
}
bool ServerFrame::getDatabaseConfig(DataBaseConfig& conf, const SDBResult& r)
{
    if (r.num_rows() > 0)
    {
        DBRow row = r[0];
        conf._id        = row["Id"];
        conf._db        = std::string(row["DBName"]);
        conf._adrs      = std::string(row["DBIp"]);
        conf._user      = std::string(row["User"]);
        conf._pwd       = std::string(row["Password"]);
        conf._port      = row["Port"];
        conf._thread_   = row["Thread"];
        return true ;
    }
    return false;
}
bool ServerFrame::getIpByname(const char* address, std::string& ip)
{
	struct hostent *hptr = gethostbyname( address );
	 hptr = gethostbyname( address );
	if( hptr )
	{
		char str[32] = { 0 };
		sprintf( str, "%u.%u.%u.%u",(hptr->h_addr_list[0][0]&0x00ff),(hptr->h_addr_list[0][1]&0x00ff),(hptr->h_addr_list[0][2]&0x00ff),(hptr->h_addr_list[0][3]&0x00ff));
		ip = str;
		return true;
	}
	else
	{
		//assert( 0 && "invalid address" );
		return false;
	}
}
bool ServerFrame::getNetConfig(net_info& _conf, const UDBResult& r)
{
	while (DBRow row = r.fetch_row()) 
	{
		_conf._id = row["ServerId"];
		_conf._port = row["Port"];
		_conf._max_connect = row["MaxConnect"];
		_conf._thread_count = row["ThreadCount"];
		_conf._time_out = row["OutTime"];
		return getIpByname(row["Ip"], _conf._ip);
	}
	return false;
}

bool ServerFrame::loadDBConfig(DataBaseConfig&_config, const char* str)
{
    JsonHelper help(str);
    if (!help.loadJson())
    {   
		return false;
	}
    const Json::Value& Root = help.root();
    LAZY_JSON_GET_STRING(_config._db,		"db",			    Root);
    LAZY_JSON_GET_STRING(_config._adrs,		"ip",			    Root);
    LAZY_JSON_GET_STRING(_config._user,		"user",		        Root);
    LAZY_JSON_GET_STRING(_config._pwd,		"password",		    Root);
    LAZY_JSON_GET_UINT(_config._port,		"port",			    Root);			
    LAZY_JSON_GET_UINT(_config._thread_	,	"thread_count",	    Root);

	return getIpByname(_config._adrs.c_str(), _config._adrs) ;
}
bool ServerFrame::loadServiceConfig(service_config& _config, const char* str)
{
    JsonHelper help(str);
    if (!help.loadJson())
    {   return false;}
    const Json::Value& Root = help.root();
    LAZY_JSON_GET_UINT(_config.id,		        "serviceid",			    Root);			
    LAZY_JSON_GET_UINT(_config.limit_player	,	"limit",	                Root);
    LAZY_JSON_GET_UINT(_config.thread_count,	"thread_count",			    Root);			
    LAZY_JSON_GET_UINT(_config.proc_interval,	"proc_interval",	        Root);
    LAZY_JSON_GET_UINT(_config.speed_,		    "max_speed",			    Root);			
    LAZY_JSON_GET_UINT(_config.msg_pool_size,	"msg_pool_size",	        Root);
    return true ;
}

void ServerFrame::signalStop()
{
	if (g_wait_stop == true)
	{
		if (g_wait_stop != _wait_stop)
		{
			setStop();
			_wait_stop = true;
		}
	}
}