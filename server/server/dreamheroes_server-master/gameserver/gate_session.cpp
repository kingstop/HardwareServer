#include "stdafx.h"
#include "gate_session.h"
#include "session.h"
GateSession::GateSession(): tcp_session( *net_global::get_io_service() )
{
	_proto_user_ptr = this;
    m_gate_id = INVALID_GAME_ID;
}

GateSession::~GateSession()
{
}

void GateSession::parseAddUser(google::protobuf::Message* p, pb_flag_type flag)
{
    message::MsgGT2GSAddUser* msg = static_cast<message::MsgGT2GSAddUser*>(p);
    if (msg)
    {
        tran_id_type t = msg->data().transid();
        account_type a = msg->data().account();
        gGSGateManager.addUser( t, a, m_gate_id);
    }
}

void GateSession::parseRemoveUser(google::protobuf::Message* p, pb_flag_type flag)
{
    message::MsgGT2GSRemoveUser* msg = static_cast<message::MsgGT2GSRemoveUser*>(p);
    if (msg)
    {
        tran_id_type t = msg->trans();
        gGSGateManager.removeUser(t);
    }
}

void GateSession::parseOffline(google::protobuf::Message* p, pb_flag_type flag)
{
    message::MsgGT2GSOfflineUser* msg = static_cast<message::MsgGT2GSOfflineUser*>(p);
    if (msg)
    {
        tran_id_type t = msg->trans();
        gGSGateManager.offlineUser(t);
    }
}

void GateSession::parseUserMsg(google::protobuf::Message* p, pb_flag_type flag)
{
	Session* pkSession = gGSGateManager.getUser(flag);
	if (pkSession)
	{
		pkSession->parsePBMessage(p);
	}
	else
	{
		Mylog::log_server(LOG_ERROR, "not found session by trans id [%u]", flag);
	}
}

void GateSession::parseGateRegister(google::protobuf::Message* p, pb_flag_type flag)
{
	message::MsgServerRegister* msg = static_cast<message::MsgServerRegister*>(p);
	if (msg)
	{
        const ::message::ServerVersion& v = msg->version();
        m_gate_id = msg->serverid();
        if (!gGSVersion.checkVersion(v.major(), v.minor(), v.build(), v.appsvn()))
        {
            Mylog::log_server(LOG_ERROR,"register Gate [%u] error, version is too old! ",m_gate_id);
            close();
        }
		gGSGateManager.addGate(m_gate_id, this);
		Mylog::log_server(LOG_INFO,"register Gate [%u] ", m_gate_id);
        message::MsgServerRegister m ; 
        m.set_serverid(gGameServer.getServerID());
        message::ServerVersion* p2 = m.mutable_version();
        p2->set_major(gGSVersion.nMajor);
        p2->set_minor(gGSVersion.nMinor);
        p2->set_build(gGSVersion.nBuild);
        p2->set_appsvn(gGSVersion.nAppSvn);
        sendPBMessage(&m);
	}
}

void GateSession::initPBModule()
{
	ProtocMsgBase<GateSession>::registerSDFun(&GateSession::send_message, &GateSession::parseUserMsg);
	ProtocMsgBase<GateSession>::registerCBFun(PROTOCO_NAME(message::MsgServerRegister), &GateSession::parseGateRegister);
    ProtocMsgBase<GateSession>::registerCBFun(PROTOCO_NAME(message::MsgGT2GSAddUser), &GateSession::parseAddUser);
    ProtocMsgBase<GateSession>::registerCBFun(PROTOCO_NAME(message::MsgGT2GSRemoveUser), &GateSession::parseRemoveUser);
    ProtocMsgBase<GateSession>::registerCBFun(PROTOCO_NAME(message::MsgGT2GSOfflineUser), &GateSession::parseOffline);
}

void GateSession::proc_message( const message_t& msg )
{
	parsePBMessage(msg.data, msg.len);
}

void GateSession::on_close( const boost::system::error_code& error )
{
	Mylog::log_server(LOG_INFO, "gate [%u] close", m_gate_id);
	gGSGateManager.removeGate(this);
	tcp_session::on_close(error);
}

