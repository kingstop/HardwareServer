#ifndef __game_client_h__
#define __game_client_h__

class GateGameClient : public tcp_client , public ProtocMsgBase<GateGameClient>
{
public:
    GateGameClient();
    ~GateGameClient();

    struct GameInfo
    {
        GameInfo()
        {
            game_id = 0 ;
            onlines = 0 ;
        }
        u8	game_id;
        u32 onlines;
    };
    const GameInfo& getGateInfo(){return m_GameInfo;}
    u32 getOnlines() const{return m_GameInfo.onlines;}
    u8  getGameId() const{return m_GameInfo.game_id;}


    static void initPBModule();
    void parseGameMsg(google::protobuf::Message* p, pb_flag_type flag);
    void parseGameRegister(google::protobuf::Message* p, pb_flag_type flag);
    void parseGameOnlines(google::protobuf::Message* p, pb_flag_type flag);
	void parseChangeGS(google::protobuf::Message* p, pb_flag_type flag);
    void addNewPlayer(tran_id_type t,  account_type a);
    void removePlayer(tran_id_type t);
    void offlinePlayer(tran_id_type t);

    virtual void on_connect();
    virtual void on_close( const boost::system::error_code& error );
    virtual void on_connect_failed( boost::system::error_code error );
    virtual void proc_message( const message_t& msg );

    void setNetInfo(const net_info& info){m_info = info;};
protected:
    net_info m_info;
    GameInfo m_GameInfo;
private:
};



#endif