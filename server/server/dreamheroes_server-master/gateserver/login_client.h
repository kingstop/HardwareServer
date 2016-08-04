#ifndef __login_client_h__
#define __login_client_h__

class GateLoginClient : public tcp_client , public ProtocMsgBase<GateLoginClient>
{
public:
	GateLoginClient();
	~GateLoginClient();

	static void initPBModule();
    void parseLoginUser(google::protobuf::Message* p, pb_flag_type flag);
    void parseLoginKick(google::protobuf::Message* p, pb_flag_type flag);


	
	virtual void on_connect();
	virtual void on_close( const boost::system::error_code& error );
	virtual void on_connect_failed( boost::system::error_code error );
	virtual void proc_message( const message_t& msg );

    void setNetInfo(const net_info& info){m_info = info;};
protected:
	net_info m_info;
private:
};

#endif