#include "stdafx.h"
#include "message/msg_gate_login.pb.h"
GateLoginClient::GateLoginClient():tcp_client( *net_global::get_io_service() )
{
	_proto_user_ptr = this;
	m_isreconnect = true;
}
GateLoginClient::~GateLoginClient()
{

}
void GateLoginClient::parseLoginUser(google::protobuf::Message* p, pb_flag_type flag)
{
    message::MsgLN2GTUserLogin* msg = static_cast<message::MsgLN2GTUserLogin*>(p);
    if (msg)
    {
        const ::message::GTLGData& d = msg->data();
        gGTUserMgr.addToWait(d.transid(), d.account());
    }
    
}
void GateLoginClient::parseLoginKick(google::protobuf::Message* p, pb_flag_type flag)
{
    message::MsgLN2GTKickUser* msg = static_cast<message::MsgLN2GTKickUser*>(p);
    if (msg)
    {
        const ::message::GTLGData& d = msg->data();
        Mylog::log_player(LOG_DEBUG, "handle kick user a:[%u], t:[%u]",d.account(), d.transid());
        gGTUserMgr.kickUser(d.transid(), d.account());
        
    }
}
void GateLoginClient::initPBModule()
{
	ProtocMsgBase<GateLoginClient>::registerSDFun(&GateLoginClient::send_message, NULL);
    ProtocMsgBase<GateLoginClient>::registerCBFun(PROTOCO_NAME(message::MsgLN2GTUserLogin),   &GateLoginClient::parseLoginUser);
    ProtocMsgBase<GateLoginClient>::registerCBFun(PROTOCO_NAME(message::MsgLN2GTKickUser),    &GateLoginClient::parseLoginKick);
}
void GateLoginClient::on_close( const boost::system::error_code& error )
{
	tcp_session::on_close( error );
	Mylog::log_server(LOG_INFO, "tcp login client close");
}

void GateLoginClient::on_connect()
{
	tcp_client::on_connect();
	Mylog::log_server(LOG_INFO, "connect LoginSever ip success!");
    const net_info& info = gGTUserServer.getNetInfo();
    message::MsgGTRegisterLG msg;
    msg.set_ip(info._ip);
    msg.set_port(info._port);

    message::MsgServerRegister* p1 = msg.mutable_gateinfo();
    p1->set_serverid(info._id);
    message::ServerVersion* p2 = p1->mutable_version();
    p2->set_major(gGTVersion.nMajor);
    p2->set_minor(gGTVersion.nMinor);
    p2->set_build(gGTVersion.nBuild);
    p2->set_appsvn(gGTVersion.nAppSvn);

    gGTUserMgr.makeRegisterData(msg);
    sendPBMessage(&msg);
    gGTUserMgr.enableGateEvent();
}
void GateLoginClient::on_connect_failed( boost::system::error_code error )
{
	 Mylog::log_server(LOG_ERROR, "connect LoginServer ip:[%s]  port:[%u] failed!", m_info._ip.c_str(), m_info._port);
}
void GateLoginClient::proc_message( const message_t& msg )
{
	parsePBMessage(msg.data, msg.len);
}