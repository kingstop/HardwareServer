#include "stdafx.h"
#include "game_session.h"

GameSession::GameSession(): tcp_session( *net_global::get_io_service() )
{
	_proto_user_ptr = this;
    m_game_id = INVALID_GAME_ID;
}

GameSession::~GameSession()
{

}

void GameSession::parseGameMsg(google::protobuf::Message* p, pb_flag_type flag)
{



}


void GameSession::parseGameRegister(google::protobuf::Message* p, pb_flag_type flag)
{
	message::MsgServerRegister* msg = static_cast<message::MsgServerRegister*>(p);
	if (msg)
	{
        const ::message::ServerVersion& v = msg->version();
        m_game_id = msg->serverid();
        if (!gDBVersion.checkVersion(v.major(), v.minor(), v.build(), v.appsvn()))
        {
            Mylog::log_server(LOG_ERROR,"register Game [%u] error, version is too old! ",m_game_id);
            close();
        }
		gDBGameManager.addGame(m_game_id, this);
		Mylog::log_server(LOG_INFO,"register Game [%u] ", m_game_id);
	}
}


void GameSession::initPBModule()
{
	ProtocMsgBase<GameSession>::registerSDFun(&GameSession::send_message, &GameSession::parseGameMsg);
	ProtocMsgBase<GameSession>::registerCBFun(PROTOCO_NAME(message::MsgServerRegister),&GameSession::parseGameRegister);
	ProtocMsgBase<GameSession>::registerCBFun(PROTOCO_NAME(message::MsgReqHeroDataGS2DB), &GameSession::parseMsgReqHeroDataGS2DB);
	ProtocMsgBase<GameSession>::registerCBFun(PROTOCO_NAME(message::MsgSaveDataGS2DB), &GameSession::parseMsgSaveHeroData);
	ProtocMsgBase<GameSession>::registerCBFun(PROTOCO_NAME(message::MsgSaveAllHeroesGS2DB), &GameSession::parseMsgSaveAllHeroesGS2DB);
	ProtocMsgBase<GameSession>::registerCBFun(PROTOCO_NAME(message::MsgVerifyToyGS2DB), &GameSession::parseMsgVerifyToy);
}


void GameSession::parseMsgSaveHeroData(google::protobuf::Message* p, pb_flag_type flag)
{
	message::MsgSaveDataGS2DB* msg = (message::MsgSaveDataGS2DB*)p;
	gFuDBFather->sDBQuestMgr.saveSqlData(msg->sql().c_str());
}

void GameSession::parseMsgReqHeroDataGS2DB(google::protobuf::Message* p, pb_flag_type flag)
{
	message::MsgReqHeroDataGS2DB* msg = (message::MsgReqHeroDataGS2DB*)p;
	gDBQuestMgr.queryHeroInfo(msg->account(), flag, m_game_id);
}

void GameSession::proc_message( const message_t& msg )
{
	parsePBMessage(msg.data, msg.len);
}

void GameSession::on_close( const boost::system::error_code& error )
{
	gDBGameManager.removeGame(this);
	tcp_session::on_close(error);
	Mylog::log_server(LOG_INFO, "server game [%u] close", m_game_id);
}


void GameSession::parseMsgVerifyToy(google::protobuf::Message* p, pb_flag_type flag)
{
	message::MsgVerifyToyGS2DB* msg = (message::MsgVerifyToyGS2DB*)p;
	gDBQuestMgr.verifyToyCDKey(flag, m_game_id, msg->account(), msg->cdkey().c_str());
}

void GameSession::parseMsgSaveAllHeroesGS2DB(google::protobuf::Message* p, pb_flag_type flag)
{
	gDBQuestMgr.saveToClose(m_game_id);
}