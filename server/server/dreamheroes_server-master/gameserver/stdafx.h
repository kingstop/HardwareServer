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
#include "version.h"
#include "event_table_object.h"
#include "event_manager.h"
#include "message/common.pb.h"
#include "message/login.pb.h"
#include "message/server_define.h"
#include "message/msgs2s.pb.h"
#include "message/msg_gate_game.pb.h"
#include "message/msg_game_db.pb.h"
#include "message/dream_hero.pb.h"
#include "server.h"
#include "gate_manager.h"
#include "gate_tcp_server.h"
#include "db_client.h"
#include "DreamHeroManager.h"
#include "GameConfig.h"
/*#include "ObjectManager.h"
#include "FRWorld.h*"
*/

#include "database.h"


std::string Utf8ToGBK(const char* strUtf8);

std::string GBKToUtf8(const char* strGBK);
enum EventTypes
{
	EVENT_UNK = 0,
	EVENT_SAVE_PLAYER_DATA_,
	EVENT_DAILY_RANK_,
	EVENT_NEWS_,
	EVENT_COLLECT_INFO_,
	EVENT_COLLECT_SESSION_INFO_
};
struct FuGameFather
{
	FuGameFather():sSystemTime(time(0))
	{
	}
	time_t          sSystemTime;
	Version         sVersion;
	GameServer      sGameServer;
	GateManager     sGateManager;
	GateTcpServer   sGateTcpServer;
	GameDBClient    sGameConnDB;
	Database        sCharacterDB;
	Database		sCenterDB;
	Database		sWorldDB;
	DreamHeroManager sDreamHeroManager;
	GameConfig       sGameConfig;
};

extern FuGameFather* gFuGameFather;
#define gGSVersion			gFuGameFather->sVersion
#define gGSServerTime		gFuGameFather->sSystemTime
#define gGameServer			gFuGameFather->sGameServer
#define gGSGateManager		gFuGameFather->sGateManager
#define gGSDBClient			gFuGameFather->sGameConnDB
#define gGSGateServer		gFuGameFather->sGateTcpServer 
#define gDBCharDatabase		gFuGameFather->sCharacterDB
#define gCenterDataBase		gFuGameFather->sCenterDB
#define gWorldDatabase		gFuGameFather->sWorldDB
#define gDreamHeroManager   gFuGameFather->sDreamHeroManager
#define gGameConfig			gFuGameFather->sGameConfig
enum
{
	_SAVE_GIRL_ = _NORMAL_THREAD + 1,
	_SAVE_GOODS_ 
};

#endif

