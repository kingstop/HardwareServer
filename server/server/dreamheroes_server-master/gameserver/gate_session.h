#ifndef __gate_session_h__
#define __gate_session_h__
class Session;
class GateSession : public tcp_session, public ProtocMsgBase<GateSession>
{
public:
	GateSession(void);
	~GateSession(void);
	static void initPBModule();
	void parseAddUser(google::protobuf::Message* p, pb_flag_type flag);
	void parseRemoveUser(google::protobuf::Message* p, pb_flag_type flag);
	void parseOffline(google::protobuf::Message* p, pb_flag_type flag);
	void parseGateRegister(google::protobuf::Message* p, pb_flag_type flag);
	void parseUserMsg(google::protobuf::Message* p, pb_flag_type flag);
	u16  getGateId() const{return m_gate_id;}
public:
	virtual void proc_message( const message_t& msg );
	virtual void on_close( const boost::system::error_code& error );
private:
	u16 m_gate_id;
	obj_ptr_map<tran_id_type, Session> m_users;
};
#endif