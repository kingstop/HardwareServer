#ifndef __stdafx_h__
#define __stdafx_h__
#include <time.h>
#include "task_thread_pool.h"
#include "io_service_pool.h"
#include "tcpsession.h"
#include "tcpserver.h"
#include "net_type.h"
#include "my_log.h"
#include "crypt.h"
#include "memory.h"
#include "database.h"
#include "event_table_object.h"
#include "event_manager.h"
#include "server_frame.h"
#include "message/server_define.h"
#include "message/login.pb.h"
#include "message/msg_gate_login.pb.h"
#include "message/msgs2s.pb.h"
//#include "message/girls.pb.h"
#include "protoc_common.h"
#include "version.h"

#include "account_manager.h"
#include "server.h"
#include "user_tcp_server.h"
#include "gate_tcp_server.h"
#include "gate_manager.h"


enum EventTypes
{
    EVENT_UNK = 0,
    EVENT_USER_SESSION_CLOSE,
	EVENT_GATE_SESSION_BALENCE_TIME
};
struct FuLoginFather
{
	FuLoginFather():sSystemTime(time(NULL))
	{

	}

	~FuLoginFather()
	{
	}
    Version         sVersion;
    time_t          sSystemTime;
    Database        sCenterDB;
    LoginServer     sLoginServer;
    AccountManager  sAccountMgr;
	UserTcpServer	sUserTcpServer;
	GateTcpServer	sGateTcpServer;
	GateManager		sGateManager;

};

extern FuLoginFather*   gFuLoginServer;
#define gLGCenterDB     gFuLoginServer->sCenterDB
#define gLoginServer    gFuLoginServer->sLoginServer
#define gLGAccountMgr   gFuLoginServer->sAccountMgr
#define gLGServerTime   gFuLoginServer->sSystemTime
#define gLGUserServer	gFuLoginServer->sUserTcpServer
#define gLGGateServer	gFuLoginServer->sGateTcpServer
#define gLGVersion      gFuLoginServer->sVersion
#define gLGGateManager	gFuLoginServer->sGateManager	
#endif