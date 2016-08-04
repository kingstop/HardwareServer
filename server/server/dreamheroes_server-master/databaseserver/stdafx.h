#ifndef __stdafx_h__
#define __stdafx_h__
#include <time.h>
#include "common_type.h"
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
#include "version.h"
#include "message/server_define.h"
#include "message/msgs2s.pb.h"
#include "message/msg_game_db.pb.h"
#include "message/msgs2c.pb.h"
#include "message/common.pb.h"
#include "message/login.pb.h"
#include "server.h"
#include "game_manager.h"
#include "game_tcp_server.h"
#include "db_quest.h"
#include "event_table_object.h"
#include "CharDatabase.h"


struct FuDBFather
{
	FuDBFather():sSystemTime(time(0))
	{
	}
	time_t          sSystemTime;
	Version         sVersion;
	DBServer        sDBServer;
	GameManager     sGameManager;
	GameTCPServer   sGameTcpServer;
	CharDatabase    sCharacterDB;
	Database		sWorldDB;
	DBQuestManager	sDBQuestMgr;
};

extern FuDBFather* gFuDBFather;
#define gDBVersion			gFuDBFather->sVersion
#define gDBServerTime		gFuDBFather->sSystemTime
#define gDBServer			gFuDBFather->sDBServer
#define gDBGameServer		gFuDBFather->sGameTcpServer    
#define gDBGameManager		gFuDBFather->sGameManager 
#define gDBCharDatabase		gFuDBFather->sCharacterDB
#define gDBWorldDatabase    gFuDBFather->sWorldDB
#define gDBQuestMgr			gFuDBFather->sDBQuestMgr

enum EventTypes
{
	EVENT_UNK = 0,
	EVENT_SAVE_TO_CLOSE,
};

#endif

