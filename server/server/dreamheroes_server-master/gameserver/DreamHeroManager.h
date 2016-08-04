#pragma once

class DreamHero;
class DreamHeroManager : public EventableObject
{
public:
	typedef std::map<account_type, DreamHero*> MAPHEROS;
	typedef std::map<std::string, account_type> MAPSTRACCPAIR;
public:
	DreamHeroManager();
	virtual ~DreamHeroManager();
	DreamHero* GetHero(account_type account);
	DreamHero* CreateHero(message::MsgHeroDataDB2GS* HeroDataMsg, account_type acc, Session* session);
	DreamHero* CreateHero(account_type acc, Session* session);
	void SaveDreamHeroes();
	bool is_save_all_heroes_ok();
	void save_all_heroes_ok();
	void CollectInfo();
	account_type getToyHero(const char* cdkey);
	void modifyToyAccount(account_type acc, const char* cdkey);
protected:
	MAPHEROS _heros;
	MAPSTRACCPAIR _toy_cdkey_account;

	bool _save_all_heroes_ok;
};

