#ifndef __db_client_h__
#define __db_client_h__
class GameDBClient : public tcp_client , public ProtocMsgBase<GameDBClient>
{
public:
	GameDBClient();
	~GameDBClient();
	static void initPBModule();
	void parseQueryChar(google::protobuf::Message* p, pb_flag_type flag);	
	void parseDBMsgDefault(google::protobuf::Message* p, pb_flag_type flag);
	void parseQueryHeroInfo(google::protobuf::Message* p, pb_flag_type flag);
	void parseNeedCreateHero(google::protobuf::Message* p, pb_flag_type flag);
	void parseMsgSaveAllHeroes(google::protobuf::Message* p, pb_flag_type flag);
	void parseMsgVerifyToy(google::protobuf::Message* p, pb_flag_type flag);
	void MsgVerifyToyError(google::protobuf::Message* p, pb_flag_type flag);

	virtual void on_connect();
	virtual void on_close( const boost::system::error_code& error );
	virtual void on_connect_failed( boost::system::error_code error );
	virtual void proc_message( const message_t& msg );
protected:
	net_info m_info;
private:
};
#endif