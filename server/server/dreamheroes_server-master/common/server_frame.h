#ifndef __server_frame_h__
#define __server_frame_h__
#include "common_type.h"
#include "database.h"
#include "net_type.h"


//////////////////////////////////////////////////////////////////////////
//  server frame ...													//
//////////////////////////////////////////////////////////////////////////
extern bool g_wait_stop;
extern u64 g_server_time;
extern u64 g_server_start_time;

class ServerFrame
{
public:
	enum
	{
		_LOGIN_LISTEN_USER_ = 0,		
		_LOGIN_LISTEN_GATE_ = 1,
		_GATE_LISTEN_USER_  = 2,
		_GAME_LISTEN_GATE_  = 3,
		_DB_LISNEN_GAME_    = 4,
	};

    enum
    {
        _FU_CHARACTER_DATABASE_,  //角色数据服
        _FU_WORLD_DATABASE_,      //游戏数据配置服
    };
	
	virtual bool init();					// init server service
	virtual bool onKey() = 0;				// on key down (command . block the main thread..) 
	void run();								// run ..
	virtual void runOnce(u32 nDiff) = 0;	// run once 
	virtual void shutDown() = 0;			// quit service
	virtual void memoryLog() = 0;			// memory information
	virtual void setStop() = 0;				// set stop service..
	virtual void checkStop() = 0;			// check service stop
	virtual void signalStop();
	static bool loadNetConfig(net_info& _conf, const char* str);
	static bool loadDBConfig(DataBaseConfig&_config, const char* str);
    static bool loadServiceConfig(service_config& _config, const char* str);
    static bool getIpByname(const char* str, std::string& ip);
	static bool getNetConfig(net_info& _conf, const UDBResult& r);
    static bool getDatabaseConfig(DataBaseConfig& conf, const SDBResult& r);
protected:
	bool _stop;
	bool _wait_stop;
	bool _print;
	u32 _service_stop_state;
private:
};

#endif