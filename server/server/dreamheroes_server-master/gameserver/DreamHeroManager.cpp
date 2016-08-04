#include "stdafx.h"
#include "DreamHeroManager.h"
#include "DreamHero.h"
#define _SAVE_COLLECT_TIME_  (10 * _TIME_SECOND_MSEL_)
DreamHeroManager::DreamHeroManager()
{
	_save_all_heroes_ok = false;
	if (gEventMgr.hasEvent(this, EVENT_COLLECT_INFO_) == false)
	{
		gEventMgr.addEvent(this, &DreamHeroManager::CollectInfo, EVENT_COLLECT_INFO_, _SAVE_COLLECT_TIME_, -1, 0);
	}
}


DreamHeroManager::~DreamHeroManager()
{
}


DreamHero* DreamHeroManager::GetHero(account_type account)
{
	DreamHero* hero = NULL;
	MAPHEROS::iterator it = _heros.find(account);
	if (it != _heros.end())
	{
		hero = it->second;
	}
	return hero;
}

DreamHero* DreamHeroManager::CreateHero(message::MsgHeroDataDB2GS* HeroDataMsg, account_type acc, Session* session)
{
	DreamHero* hero = GetHero(acc);
	if (hero == NULL)
	{
		hero = new DreamHero();
		const message::MsgHeroData* entry_msg = HeroDataMsg->mutable_data();
		hero->set_info(HeroDataMsg);
		hero->set_account(acc);
		_heros.insert(MAPHEROS::value_type(hero->get_account(), hero));
	}	
	hero->set_session(session);
	hero->set_account(acc);
	return hero;
}

DreamHero* DreamHeroManager::CreateHero(account_type acc, Session* session)
{
	DreamHero* hero = GetHero(acc);
	if (hero == NULL)
	{
		hero = new DreamHero();
		hero->LoadFromConfig();
	}
	hero->set_session(session);
	hero->set_account(acc);
	_heros.insert(MAPHEROS::value_type(hero->get_account(), hero));
	return hero;
}

account_type DreamHeroManager::getToyHero(const char* cdkey)
{
	account_type acc = 0;
	MAPSTRACCPAIR::iterator it = _toy_cdkey_account.find(cdkey);
	if (it != _toy_cdkey_account.end())
	{
		acc = it->second;
	}
	return acc;
}

void DreamHeroManager::SaveDreamHeroes()
{
	MAPHEROS::iterator it = _heros.begin();
	for (; it != _heros.end(); ++ it)
	{
		DreamHero* hero = it->second;
		hero->SaveHero();
	}
	Mylog::log_server(LOG_INFO, "game server save all heroes count[%d]!", _heros.size());
	message::MsgSaveAllHeroesGS2DB msg;
	gGSDBClient.sendPBMessage(&msg, 0);	
}


void DreamHeroManager::CollectInfo()
{
	int offline_count = 0;
	int online_count = 0;
	MAPHEROS::iterator it = _heros.begin();
	for (; it != _heros.end(); ++ it)
	{
		DreamHero* hero = it->second;
		if (hero)
		{
			if (hero->is_online())
			{
				online_count++;
			}
			else
			{
				offline_count++;
			}			
		}
	}
	int current_player = offline_count + online_count;

	Mylog::log_server(LOG_INFO, "current player count[%d], online player count[%d], offline player count[%d]", current_player, online_count, offline_count);
}


bool DreamHeroManager::is_save_all_heroes_ok()
{
	return _save_all_heroes_ok;
}

void DreamHeroManager::save_all_heroes_ok()
{
	_save_all_heroes_ok = true;
}

void DreamHeroManager::modifyToyAccount(account_type acc, const char* cdkey)
{
	_toy_cdkey_account[cdkey] = acc;
}