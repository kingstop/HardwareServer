#pragma once
class DreamHero : public EventableObject
{
public:
	typedef std::map<u64, message::MsgEquipData> HEROEQUIPS;
	typedef std::map<std::string, message::MsgToyData> HEROTOYS;
public:
	DreamHero();
	virtual ~DreamHero();
	message::MsgHeroData get_info();
	u64 get_account();
	void set_account(u64 account);
	void set_info(const message::MsgHeroDataDB2GS* info);
	void modify_suit(int suit_config, const char* szname);
	void set_session(Session* session);
	void set_level(int level);
	void set_name(const char* name);
	int get_level();
	const char* get_name();
	Session* get_session();
	void SaveHero();
	void StartSave();
	void StopSave();
	void LoadFromConfig();
	void SendClientInit();
	void RemoveToyByCDKey(const char* cd_key);
	void set_online(bool online);
	bool is_online();
public:
	void ModifySuit(const message::C2SModifySuitReq* msg);
	void DelSuit(const message::C2SDelSuitReq* msg);
	void VerifyToy(message::MsgVerifyToyDB2GS* msg);
	void VerifyToy(message::MsgVerifyToyErrorDB2GS* msg);
	void VerifyToyCDKey(message::C2SVerifyToyCDKeyReq* msg);
	void EquipLevelUp(message::S2CEquipLevelUpReq* msg);
protected:
	void sendPBMessage(google::protobuf::Message* p);
protected:
	message::MsgHeroData _info;
	HEROEQUIPS _hero_equips;
	HEROTOYS _hero_toys;
	account_type _account;
	Session* _session;
	bool _online;
};

