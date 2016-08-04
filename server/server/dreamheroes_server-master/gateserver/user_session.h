#ifndef _USER_SESSION_HEAD
#define _USER_SESSION_HEAD


class UserSession : public tcp_session, public ProtocMsgBase<UserSession>, public EventableObject
{
public:
	enum
	{
		_disable_,
        _wait_account_,
        _connect_,
        _wait_close_,
	};
	UserSession(void);
	~UserSession(void);

	// ×¢²áPBº¯ÊýÏûÏ¢
	static void initPBModule();
	void parseLoginGame(google::protobuf::Message* p, pb_flag_type flag);
    void parseExitGame(google::protobuf::Message* p, pb_flag_type flag);
	void parseGameMsg(google::protobuf::Message* p, pb_flag_type flag);

    void setState(u8 s);
    u8   getState() const {return m_state ;}


    void setGSid(u16 gs){m_gs_id = gs;}
    u16  getGSid() const{return m_gs_id ;}
    
    void setClose(){close();}
    u32  getTrans() const {return m_tranid;}
public:
	virtual void on_accept( tcp_server* p );
	virtual void on_close( const boost::system::error_code& error );
	virtual void proc_message( const message_t& msg );
	virtual void reset();
	virtual void run();
private:
	u32 m_tranid;
    u16 m_gs_id;
	u8  m_state;
};
/* 
bool ProtocMsgBase<UserSession>::sendPBMessage(google::protobuf::Message* msg, pb_flag_type flag )
{
	//Mylog::log_player(LOG_INFO, "send message [%s] to client", msg->GetTypeName().c_str());
	return ProtocBufferCommon::pushProtocMessage< ProtocMsgBase<T>>(msg, flag, this, &ProtocMsgBase<T>::sendPBBuffer);
}

*/
#endif 
