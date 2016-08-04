#ifndef __stdafx_h__
#define __stdafx_h__
#include <time.h>
#include "task_thread_pool.h"
#include "io_service_pool.h"
#include "tcpsession.h"
#include "tcpclient.h"
#include "tcpserver.h"
#include "net_type.h"
#include "crypt.h"
#include "memory.h"
#include "database.h"
#include "server_frame.h"
#include "protoc_common.h"
#include "event_table_object.h"
#include "event_manager.h"
#include "message/server_define.h"
#include "message/msg_gate_login.pb.h"
#include "message/msgs2s.pb.h"
#include "message/msg_gate_game.pb.h"
//#include "message/girls.pb.h"
#include "version.h"
#include "login_client.h"
#include "game_client.h"
#include "server.h"
#include "user_tcp_server.h"
#include "user_manager.h"
#include "game_manager.h"

enum EventTypes
{
    EVENT_UNK = 0,
    EVENT_USER_SESSION_CLOSE,
    EVENT_REMOVE_WAIT_CONNECT,
    EVENT_REMOVE_RECONNECT,
    EVENT_TELL_LOGIN_GATE_INFO,
};

struct FuGateFather
{
	FuGateFather():sSystemTime(0)
	{

	}

	~FuGateFather()
	{
		
	}
    Version             sVersion;
	time_t				sSystemTime;
	GateServer			sGateServer;
	GateLoginClient	    sLoginClient;
    UserTcpServer       sUserTcpServer;
    UserManager         sUserManager;
    GameManager         sGameManager;
};

extern FuGateFather*   gFuGateServer;
#define gGateServer		gFuGateServer->sGateServer
#define gGTServerTime   gFuGateServer->sSystemTime
#define gGTLoginClient  gFuGateServer->sLoginClient
#define gGTUserServer   gFuGateServer->sUserTcpServer 
#define gGTVersion      gFuGateServer->sVersion
#define gGTUserMgr		gFuGateServer->sUserManager
#define gGTGameMgr		gFuGateServer->sGameManager
#endif
