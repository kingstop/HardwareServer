#include "stdafx.h"
#include "user_session.h"
#include "message/login.pb.h"
#include "account_manager.h"

enum
{
    _SESSION_WAIT_ACCOUNT_ = 30* _TIME_SECOND_MSEL_,
    _SESSION_CHECK_ACCOUNT_ = 60 * _TIME_SECOND_MSEL_,
    _SESSION_WAIT_GATE_ = 60 * _TIME_SECOND_MSEL_,
    _SESSION_WAIT_CLOSE = 5 * _TIME_SECOND_MSEL_,
};

void UserLoginSession::initPBModule()
{
	ProtocMsgBase<UserLoginSession>::registerSDFun(&UserLoginSession::send_message, NULL);
	ProtocMsgBase<UserLoginSession>::registerCBFun(PROTOCO_NAME(message::LoginRequest),  &UserLoginSession::parseLoginGame);
	ProtocMsgBase<UserLoginSession>::registerCBFun(PROTOCO_NAME(message::RegisterAccountRequest),  &UserLoginSession::parseRegister);
}
void UserLoginSession::parseLoginGame(google::protobuf::Message* p, pb_flag_type flag)
{
	message::LoginRequest* msg = static_cast<message::LoginRequest*>(p);
	if (msg)
	{
		//static int login_account = 0;
		//login_account ++;

        //Mylog::log_player( LOG_INFO, "Handle Account[%s] Login [%d]!", msg->name().c_str(), login_account);
		gLGAccountMgr.checkAccount(msg->name(), msg->pwd(), this, CheckAcct::_login_check_, "check account");
//         message::CharMove m;
//         m.set_charid(1);
//         m.set_dir(10.f);
//         m.set_x(10.f);
//         m.set_x(10.f);
//         m.set_timestamp(gLGServerTime);
//         sendPBMessage(&m);
	}
	
}


void UserLoginSession::parseRegister(google::protobuf::Message* p, pb_flag_type flag)
{
	
	message::RegisterAccountRequest* msg = (message::RegisterAccountRequest *)p;
	if (msg)
	{
		gLGAccountMgr.checkAccount(msg->name(), msg->pwd(), this, CheckAcct::_new_acc_check_, "new account");
	}
	

}


UserLoginSession::UserLoginSession(void) : tcp_session( *net_global::get_io_service() )
{
	m_accid = 0;
	_proto_user_ptr = this;
}

UserLoginSession::~UserLoginSession(void)
{
}
void UserLoginSession::setState(u8 s)
{
    m_state = s;
    switch(s)
    {
    case _disable_:
        {
            gEventMgr.removeEvents(this, EVENT_USER_SESSION_CLOSE);
        }break;
    case _wait_data_:
        {
            gEventMgr.addEvent(this, &UserLoginSession::setClose,EVENT_USER_SESSION_CLOSE, _SESSION_WAIT_ACCOUNT_ ,1,EVENT_FLAG_DELETES_OBJECT);
        }break;
    case  _checking_data_:
        {
            if (gEventMgr.hasEvent(this, EVENT_USER_SESSION_CLOSE))
            {
                gEventMgr.modifyEventTimeAndTimeLeft(this, EVENT_USER_SESSION_CLOSE, _SESSION_CHECK_ACCOUNT_);
            }
    
        }break;
    case _wait_gate_:
        {
            if (gEventMgr.hasEvent(this, EVENT_USER_SESSION_CLOSE))
            {
                gEventMgr.modifyEventTimeAndTimeLeft(this, EVENT_USER_SESSION_CLOSE, _SESSION_WAIT_GATE_);
            }
        }break;
    case _wait_close_:
        {
            if (gEventMgr.hasEvent(this, EVENT_USER_SESSION_CLOSE))
            {
                gEventMgr.modifyEventTimeAndTimeLeft(this, EVENT_USER_SESSION_CLOSE, _SESSION_WAIT_CLOSE);
            }
        }break;
    default:
        break;
    }
}
void UserLoginSession::on_accept( tcp_server* p )
{
    setState(_wait_data_);
	tcp_session::on_accept( p );
    Mylog::log_player(LOG_INFO, "add a user session");
}

void UserLoginSession::on_close( const boost::system::error_code& error )
{
    
    setState(_disable_);
	
	tcp_session::on_close( error );
	Mylog::log_player(LOG_INFO, "remove a user session");
}


void UserLoginSession::proc_message( const message_t& msg )
{
	parsePBMessage(msg.data, msg.len);
}

void UserLoginSession::reset()
{
	m_state = _disable_;
	m_accid = 0;
	tcp_session::reset();
}

void UserLoginSession::run()
{
	tcp_session::run();
}


