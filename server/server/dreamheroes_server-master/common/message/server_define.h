#ifndef __server__define_h__
#define __server__define_h__

#include "booth.pb.h"

#include "common.pb.h"

#include "login.pb.h"

#include "msgs2c.pb.h"

#include "msgs2s.pb.h"

#include "msg_gate_login.pb.h"

#include "msg_gate_game.pb.h"

#include "msg_game_db.pb.h"



/************************************************************************/

/*              server define                                           */

/************************************************************************/

#define     INVALID_ACCOUNT         0

#define     INVALID_GATE_ID         0

#define     INVALID_GAME_ID         0

#define     INVALID_TRANS_ID        0

#define     INVALID_PLAYER_ID       0

#define		INVALID_CITY_ID			0

#define     INVALID_SCENE_ID		0

#define		INVALID_MAP_ID			0	

#define     MAX_GATE_ID             0xff

#define     MAX_GAME_ID             0xff

#define     INVALID_MOLE_FARM_ID    ""

#define     SERVER_CONFIG           "conf/server.json" 

#define     CENTER_DATABASE_CONFIG  "../server_conf/database.json"

#define     LOG4CXX_LOG_CONFIG      "../server_conf/log.config"



enum

{

	_TIME_SECOND_ = 1,

	_TIME_MINUTE_ = 60,

	_TIME_HOUR_ = _TIME_MINUTE_ * 60,

	_TIME_DAY_ = _TIME_HOUR_ * 24,



	_TIME_SECOND_MSEL_ = 1000,

};



enum

{

	_DB_FOUND_PLAYERINFO__,

	_DB_NOT_FOUND_PLAYERINFO_,

};

enum

{

	_MAIL_NOT_READ_,

	_MAIL_READED_,

};

enum

{

	_PLAYER_INFO_FLAG_INIT_MAP_POS_,

};



typedef u64  account_type ;

typedef u32  tran_id_type;

#endif

