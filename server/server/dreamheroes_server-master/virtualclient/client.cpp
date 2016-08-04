#include "stdafx.h"
#include "client.h"
Client::Client(u32 index, const net_info& info)
:tcp_client(*net_global::get_io_service()),m_testIndex(index), m_info(info), m_client_state(_client_init_), m_transid(INVALID_TRANS_ID), m_MoveStateEnable(false)
{
    _proto_user_ptr = this;
	char buff[128];
  // connect(m_info._ip.c_str(), m_info._port);
	sprintf(buff, "%s_%u", Instance.config_.get_virtual_account(), m_testIndex);
	_acc = buff;
	Instance.client_manage_.addOfflineClient(this);
	m_ready_connect_gate = false;
}
Client::~Client()
{

}

bool Client::reConnect()
{
   connect(m_info._ip.c_str(), m_info._port);
   return true;
}
void Client::initPBModule()
{
    ProtocMsgBase<Client>::registerSDFun(&Client::send_message, NULL);
    ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::LoginResponse), &Client::parseLoginResult);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CMsgHeroInit), &Client::parseMsgHeroInit);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CMsgHeroEquip), &Client::parseMsgHeroEquip);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CMsgToyInit), &Client::parseMsgToyInit);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CMsgEquipInit), &Client::parseMsgEquipInit);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CModifySuitNameACK), &Client::parseModifySuitNameACK);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CModifySuitErrorACK), &Client::parseModifySuitErrorACK);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CModifySuitACK), &Client::parseModifySuitACK);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CDelSuitACK), &Client::parseDelSuitACK);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CVerifyToyCDKeyErrorACK), &Client::parseVerifyToyCDKeyErrorACK);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CVerifyToyCDKeyACK), &Client::parseVerifyToyCDKeyACK);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CEquipLevelUpReq), &Client::parseEquipLevelUpReq);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CEquipLevelUpACK), &Client::parseEquipLevelUpACK);
	ProtocMsgBase<Client>::registerCBFun(PROTOCO_NAME(message::S2CEquipLevelUpErrorACK), &Client::parseEquipLevelUpErrorACK);
}
void Client::parseClientChar(google::protobuf::Message* p, pb_flag_type flag)
{
    //clientmsg::MapScreenRefreshCharacter* msg = static_cast<clientmsg::MapScreenRefreshCharacter*>(p);
    //if (msg)
    //{
    //    if (!m_MoveStateEnable)
    //    {
    //        const MapCharInfo& m = msg->mapinfo();
    //        m_MoveInfo.set_charid( m_PlayerData.charinfo().charid());
    //        m_MoveInfo.set_x(m.x());
    //        m_MoveInfo.set_y(m.y());
    //        m_MoveInfo.set_dir(m.dir());
    //        m_MoveInfo.set_speed(2.0f);
    //        m_MoveStateEnable = true;

    //        gEventMgr.addEvent(this, &Client::moveRand, EVENT_ROBOT_RAND_MOVE, 2*1000, 0 ,0);
    //        gEventMgr.addEvent(this, &Client::moveSend, EVENT_ROBOT_RAND_SEND, 200, 0 ,0);
    //    }
    //}
}
void Client::parseLoginResult(google::protobuf::Message* p, pb_flag_type flag)
{
	message::LoginResponse* msg = static_cast<message::LoginResponse*>(p);
	message::enumLoginResult entemp = msg->result();
	bool b_ret = true;
	if (entemp == message::enumLoginResult_Success)
	{
		if (msg)
		{
			m_transid = msg->user_account();
			m_info._ip = msg->gate_ip();
			m_info._port = msg->gate_port();
			m_ready_connect_gate = true;
			close();
		}
	}
	else
	{
		b_ret = false;
	}
}
void Client::parseClientInit(google::protobuf::Message* p, pb_flag_type flag)
{

}
void Client::update(u32 nDiff)
{
    run_no_wait();

}
void Client::moveSend()
{

}
void Client::moveRand()
{

}
void Client::on_connect()
{
	Instance.client_manage_.removeFromReconn(this);
    tcp_client::on_connect();
	//Instance.client_manage_.addOnlineClient(this);
	
    switch(m_client_state)
    {
    case _client_init_:
    case _client_wait_login_:
        {
            m_client_state = _client_conn_login_;
			message::LoginRequest msg;
            char buff[20] = {0};
            msg.set_name(_acc.c_str());
            msg.set_pwd("123456");
            sendPBMessage(&msg);
			//static int login = 0;
			//login ++;
            //Mylog::log_player(LOG_INFO, "login account[%s] count[%d]", buff, login);
			
        }break;
    case _client_wait_gate_:
        {
            m_client_state = _client_connet_gate_;
			message::LoginGame msg;
            msg.set_user_account(m_transid);
            sendPBMessage(&msg);

			//static int gate = 0;
			//gate ++;
            //Mylog::log_player(LOG_INFO, "Gate trans[%u] count[%d]", m_transid, gate);
        }break;
    default:
        break;    
    }
	
}


const char* Client::getAcc()
{
	return _acc.c_str();
}
void Client::on_close( const boost::system::error_code& error )
{
    tcp_client::on_close(error);
	Instance.client_manage_.addOfflineClient(this);
	
    switch(m_client_state)
    {
    case _client_conn_login_:
        {
			if (m_ready_connect_gate == true)
			{
				m_client_state = _client_wait_gate_;
				connect(m_info._ip.c_str(), m_info._port);
				Mylog::log_player(LOG_INFO, "connect gate trans[%u]", m_transid);

			}
        }
		break;   
	case  _client_connet_gate_:
		{
			
		}
		break;
    }
	Instance.client_manage_.addOfflineClient(this);
	
}
void Client::on_connect_failed( boost::system::error_code error )
{
	
	if (m_client_state == _client_connet_gate_)
	{
		Instance.client_manage_.addToReconn(this);
	}
	else
	{

	}
   // gCLClientTese.addToReconn(this);
}
void Client::proc_message( const message_t& msg )
{
    parsePBMessage(msg.data, msg.len);
}


void Client::parseMsgHeroInit(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CMsgHeroInit* msg = (message::S2CMsgHeroInit*)p;
	Instance.client_manage_.addOnlineClient(this);
	
}

void Client::parseMsgHeroEquip(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CMsgHeroEquip* msg = (message::S2CMsgHeroEquip*)p;
}

void Client::parseMsgToyInit(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CMsgToyInit* msg = (message::S2CMsgToyInit*)p;
}

void Client::parseMsgEquipInit(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CMsgEquipInit* msg = (message::S2CMsgEquipInit*)p;
}

void Client::parseModifySuitNameACK(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CModifySuitNameACK* msg = (message::S2CModifySuitNameACK*)p;
}

void Client::parseModifySuitErrorACK(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CVerifyToyCDKeyErrorACK* msg = (message::S2CVerifyToyCDKeyErrorACK*)p;
}

void Client::parseModifySuitACK(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CModifySuitACK* msg = (message::S2CModifySuitACK*)p;
}

void Client::parseDelSuitACK(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CDelSuitACK* msg = (message::S2CDelSuitACK*)p;
}


void Client::parseVerifyToyCDKeyErrorACK(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CVerifyToyCDKeyErrorACK* msg = (message::S2CVerifyToyCDKeyErrorACK*)p;
}

void Client::parseVerifyToyCDKeyACK(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CVerifyToyCDKeyACK* msg = (message::S2CVerifyToyCDKeyACK*)p;
}

void Client::parseEquipLevelUpReq(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CEquipLevelUpReq* msg = (message::S2CEquipLevelUpReq*)p;
}

void Client::parseEquipLevelUpACK(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CEquipLevelUpACK* msg = (message::S2CEquipLevelUpACK*)p;
}

void Client::parseEquipLevelUpErrorACK(google::protobuf::Message* p, pb_flag_type flag)
{
	message::S2CEquipLevelUpErrorACK* msg = (message::S2CEquipLevelUpErrorACK*)p;
}

void Client::SetTestIndex(int index)
{

}