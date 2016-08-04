#ifndef __player_session_h__
#define __player_session_h__
class DreamHero;
class Session :PUBLIC_BASE_OBJECT(Session)
{
	REGISTER_POOL_INFO(Session, 100, 0)
public:
	enum
	{
		_session_online_,
		_session_offline_,
	};
	void prasePBDefault(google::protobuf::Message* p);
	//void parseC2SSaveMapRequest(google::protobuf::Message* p);
	//void parseC2SHeroMapListRequest(google::protobuf::Message* p);
	//void parseC2SHeroMapData(google::protobuf::Message* p);
	//void parseGirlInfoRequest(google::protobuf::Message* p);
	//void parseSignRequest(google::protobuf::Message* p);
	//void parseModifyInfoRequest(google::protobuf::Message* p);
	//void parseGoodListReq(google::protobuf::Message* p);
	//void parseBuyGoodReq(google::protobuf::Message* p);
	//void parseGirlsListRankReq(google::protobuf::Message* p);
	//void parseGirlsListReq(google::protobuf::Message* p);
	//void parseModifyGirlPerformance(google::protobuf::Message* p);
	//void parseAddAccountReq(google::protobuf::Message* p);
	//void parseReservationReq(google::protobuf::Message* p);
	//void parseReservationInfoListReq(google::protobuf::Message* p);
	//void parseAdviceReq(google::protobuf::Message* p);
	//void parseAdviceListReq(google::protobuf::Message* p);
	//void parseModifyNewsReq(google::protobuf::Message* p);
	//void parseModifyPasswordReq(google::protobuf::Message* p);
	//void parseReceiveReservationReq(google::protobuf::Message* p);
	//void parseAddGoodInfoReq(google::protobuf::Message* p);
	//void parseGoodsCDKEYListReq(google::protobuf::Message* p);
	
	static void registerPBCall();
public:
	void praseDBQueryHeroInfo(message::MsgHeroDataDB2GS* HeroDataMsg);
	void parseDBQueryNeedCreateHero();
	DreamHero* get_dream_hero();
public:
	Session(tran_id_type t, account_type a, u16 gate);
	~Session();
	void close();
	void setReconnet();
	void setWaitReconnet();
	DreamHero* getDreamHero() { return _dream_hero; }
	u16  getGateId() const {return m_gate ;}
	tran_id_type getTranId() const {return m_tranid ;}
	account_type getAccount() const {return m_account ;}
	void sendPBMessage(google::protobuf::Message* p);
	void parsePBMessage(google::protobuf::Message* p);
	void parseModifySuitName(google::protobuf::Message* p);
	void parseModifySuit(google::protobuf::Message* p);
	void parseDelSuit(google::protobuf::Message* p);
	void parseC2SVerifyToyCDKeyReq(google::protobuf::Message* p);
	int getState();

protected:
	tran_id_type m_tranid;
	account_type m_account;
	u16 m_gate;
	u8  m_state;
	DreamHero* _dream_hero;
private:
};
#endif

