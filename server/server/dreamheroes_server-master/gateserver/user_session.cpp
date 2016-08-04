#include "stdafx.h"
#include "user_session.h"
#include "message/login.pb.h"

enum
{
    _WAIT_ACCOUNT_DATA_TIME_ =  20 * _TIME_SECOND_MSEL_, 
    _WAIT_CLOSE_TIME_   = 1 * _TIME_SECOND_MSEL_,
};

void UserSession::setState(u8 s)
{
    m_state = s;
	//Mylog::log_player(LOG_DEBUG, "change user[%u] stae[%u]", m_tranid, m_state);
    switch(s)
    {
    case _disable_:
        {
            m_tranid = INVALID_TRANS_ID;
            m_gs_id = INVALID_GAME_ID;

            gEventMgr.removeEvents(this, EVENT_USER_SESSION_CLOSE);
        }break;
    case _wait_account_:
        {
            gEventMgr.addEvent(this, &UserSession::setClose,EVENT_USER_SESSION_CLOSE, _WAIT_ACCOUNT_DATA_TIME_ ,1,EVENT_FLAG_DELETES_OBJECT);
        }break;
    case  _connect_:
        {
            if (gEventMgr.hasEvent(this, EVENT_USER_SESSION_CLOSE))
            {
                gEventMgr.removeEvents(this, EVENT_USER_SESSION_CLOSE);
            }

        }break;
    case _wait_close_:
        {
            if (gEventMgr.hasEvent(this, EVENT_USER_SESSION_CLOSE))
            {
                gEventMgr.modifyEventTimeAndTimeLeft(this, EVENT_USER_SESSION_CLOSE, _WAIT_CLOSE_TIME_);
            }else
            {
                gEventMgr.addEvent(this, &UserSession::setClose,EVENT_USER_SESSION_CLOSE, _WAIT_CLOSE_TIME_ ,1,EVENT_FLAG_DELETES_OBJECT);
            }
        }
    default:
        break;
    }
}

void UserSession::initPBModule()
{
	ProtocMsgBase<UserSession>::registerSDFun(&UserSession::send_message, &UserSession::parseGameMsg);
	ProtocMsgBase<UserSession>::registerCBFun(PROTOCO_NAME(message::LoginGame),  &UserSession::parseLoginGame);
	//ProtocMsgBase<UserSession>::registerCBFun(PROTOCO_NAME(message::RegisterAccountRequest),  &UserSession::r);
   //ProtocMsgBase<UserSession>::registerCBFun(PROTOCO_NAME(message::ClientExit),  &UserSession::parseExitGame);
}
void UserSession::parseGameMsg(google::protobuf::Message* p, pb_flag_type flag)
{
    GateGameClient* pkGame = gGTGameMgr.getGameClient(m_gs_id);
	if (pkGame)
	{   
		pkGame->sendPBMessage(p, m_tranid);
	}
    //Mylog::log_player(LOG_DEBUG,"user[%u] send message[%s] to Game server", m_tranid, p->GetTypeName().c_str());
}
void UserSession::parseExitGame(google::protobuf::Message* p, pb_flag_type flag)
{
    setState(UserSession::_wait_close_);
}

void UserSession::parseLoginGame(google::protobuf::Message* p, pb_flag_type flag)
{
	message::LoginGame* msg = static_cast<message::LoginGame*>(p);
	if (msg)
	{
        if (_connect_ == m_state)
        {
            //Mylog::log_player(LOG_ERROR, "client [%u] is connect ...", m_tranid);
            return ;
        }
		tran_id_type t = (tran_id_type)msg->user_account();
        if(gGTUserMgr.checkConn(t, this))
        {   m_tranid = t;}
	}
	
}
UserSession::UserSession(void) : tcp_session( *net_global::get_io_service() )
{
	m_tranid = INVALID_TRANS_ID;
    m_state = _disable_;
    m_gs_id = INVALID_GAME_ID;
	_proto_user_ptr = this;
}

UserSession::~UserSession(void)
{
}

void UserSession::on_accept( tcp_server* p )
{
    setState(_wait_account_);
	tcp_session::on_accept( p );
}

void UserSession::on_close( const boost::system::error_code& error )
{
	Mylog::log_player(LOG_DEBUG, "user [%u] close , error code[%s] ", m_tranid, error.message().c_str());
    gGTUserMgr.checkClose(this);
    setState(_disable_);
	tcp_session::on_close( error );
}

void UserSession::proc_message( const message_t& msg )
{
	parsePBMessage(msg.data, msg.len);
}

void UserSession::reset()
{
    m_tranid = INVALID_TRANS_ID;
    m_state = _disable_;
    m_gs_id = INVALID_GAME_ID;
	tcp_session::reset();
}

void UserSession::run()
{
	tcp_session::run();
}


