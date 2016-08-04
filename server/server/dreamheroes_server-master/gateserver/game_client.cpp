#include "stdafx.h"
#include "user_session.h"
GateGameClient::GateGameClient():tcp_client( *net_global::get_io_service() )
{
    m_isreconnect = true;
    _proto_user_ptr = this;
}
GateGameClient::~GateGameClient()
{

}
void GateGameClient::offlinePlayer(tran_id_type t)
{
    message::MsgGT2GSOfflineUser msg;
    msg.set_trans(t);
    sendPBMessage(&msg);
    //Mylog::log_player(LOG_INFO, "off line user[%u] from game server [%u]", t, m_GameInfo.game_id);
}
void GateGameClient::addNewPlayer(tran_id_type t,  account_type a)
{
    message::MsgGT2GSAddUser msg;
    message::GTLGData* p = msg.mutable_data();
    p->set_account(a);
    p->set_transid(t);
    sendPBMessage(&msg);
	static int new_user_count = 0;
	new_user_count ++;
   // Mylog::log_player(LOG_INFO, "add new user[%u] to game server [%u] new user count [%d]", t, m_GameInfo.game_id, new_user_count);
}
void GateGameClient::removePlayer(tran_id_type t)
{
    message::MsgGT2GSRemoveUser msg;
    msg.set_trans(t);
    sendPBMessage(&msg);
    //Mylog::log_player(LOG_INFO, "quit user[%u] from server [%u]", t, m_GameInfo.game_id);
}
void GateGameClient::parseGameMsg(google::protobuf::Message* p, pb_flag_type flag)
{
	UserSession* pksession = gGTUserMgr.getUserSession(flag);
	if (pksession)
	{
		pksession->sendPBMessage(p, flag);
        //Mylog::log_player(LOG_DEBUG, "Game Server Send Msg[%s] to user [%u]", p->GetTypeName().c_str(), flag);
	}
	else
	{
		//Mylog::log_server(LOG_DEBUG, "send[%s] msg failed, can not find session id[%u]", p->GetTypeName().c_str(), flag);
	}
    
}
void GateGameClient::parseChangeGS(google::protobuf::Message* p, pb_flag_type flag)
{
	message::MsgDB2GTChangeGS* msg = static_cast<message::MsgDB2GTChangeGS*>(p);
	UserSession* pksession = gGTUserMgr.getUserSession(flag);
	
	if (msg && pksession)
	{
		GateGameClient* pkNew = gGTGameMgr.getGameClient(msg->gameserverid());
		if (!pkNew)
		{
			pksession->setClose();
			Mylog::log_server(LOG_ERROR, "change server not found the gmae server [%u]", msg->gameserverid());
		}else
		{
			//切换到目标图的GS上
			removePlayer( pksession->getTrans());
			account_type a = gGTUserMgr.getAccount(pksession->getTrans());
			if (a != INVALID_ACCOUNT)
			{
				pksession->setGSid(msg->gameserverid());
				pkNew->addNewPlayer(pksession->getTrans(), gGTUserMgr.getAccount(pksession->getTrans()));
			}else
			{
				pksession->setClose();
				Mylog::log_server(LOG_ERROR, "change server not found the Account ");
			}
		}
	}
}
void GateGameClient::parseGameRegister(google::protobuf::Message* p, pb_flag_type flag)
{
    message::MsgServerRegister* msg = static_cast<message::MsgServerRegister*>(p);
    if (msg)
    {
        m_GameInfo.game_id = msg->serverid();
        gGTGameMgr.addActiveGameClient(this, m_GameInfo.game_id);
    }
}
void GateGameClient::parseGameOnlines(google::protobuf::Message* p, pb_flag_type flag)
{
    message::MsgGS2GTOnlines* msg = static_cast<message::MsgGS2GTOnlines*>(p);
    if (msg)
    {
        m_GameInfo.onlines = msg->onlines();
    }
}
void GateGameClient::initPBModule()
{
    ProtocMsgBase<GateGameClient>::registerSDFun(&GateGameClient::send_message, &GateGameClient::parseGameMsg);
    ProtocMsgBase<GateGameClient>::registerCBFun(PROTOCO_NAME(message::MsgServerRegister),   &GateGameClient::parseGameRegister);
    ProtocMsgBase<GateGameClient>::registerCBFun(PROTOCO_NAME(message::MsgGS2GTOnlines),     &GateGameClient::parseGameOnlines);
	ProtocMsgBase<GateGameClient>::registerCBFun(PROTOCO_NAME(message::MsgDB2GTChangeGS),     &GateGameClient::parseChangeGS);
	
}
void GateGameClient::on_close( const boost::system::error_code& error )
{
	Mylog::log_server(LOG_INFO, "game [%u] client close", m_GameInfo.game_id);
    gGTGameMgr.removeActiveGameClient(this);
    tcp_session::on_close( error );
   
}

void GateGameClient::on_connect()
{
    tcp_client::on_connect();
    Mylog::log_server(LOG_INFO, "connect game server [%s]  id [%u] success!" , get_remote_address_string().c_str() ,m_GameInfo.game_id );
	service_config conf = gGateServer.getServiceConf();
	message::MsgServerRegister msg;
	msg.set_serverid(conf.id);
	message::ServerVersion* p2 = msg.mutable_version();
	p2->set_major(gGTVersion.nMajor);
	p2->set_minor(gGTVersion.nMinor);
	p2->set_build(gGTVersion.nBuild);
	p2->set_appsvn(gGTVersion.nAppSvn);
	sendPBMessage(&msg);
}
void GateGameClient::on_connect_failed( boost::system::error_code error )
{
    Mylog::log_server(LOG_ERROR, "connect GameServer ip:[%s] port:[%u] id[%u] failed!", m_info._ip.c_str(), m_info._port, m_GameInfo.game_id);
}
void GateGameClient::proc_message( const message_t& msg )
{
    parsePBMessage(msg.data, msg.len);
}