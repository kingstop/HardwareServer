#include "stdafx.h"
#include "db_quest.h"
#include "game_session.h"
#include <stdlib.h>
#include <stdio.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#define _SAVE_TO_CLOSE_TIME_ (15 * _TIME_SECOND_MSEL_)
const char* newGUID()  
{  
	static std::string stc_string;
	static char buf[64] = {0};
	using namespace std;
	using namespace boost::uuids;
	random_generator gen;
	uuid ranuuid = gen();
	stringstream ss;
	ss << ranuuid;
	ss >> stc_string;
	return stc_string.c_str();  
} 
enum
{
	_QUERY_HERO_INFO_ = _NORMAL_THREAD + 1,
	_QUERY_HERO_MAP_INFO_,
	_SAVE_HERO_INFO_,
	_QUERRY_VERIFY_TOY_CDKEY_

};
struct tgHeroData
{
	tgHeroData(account_type acc, tran_id_type tran, u16 gs)
	{
		account = acc;
		tranid = tran;
		gsid = gs;
	}
	tgHeroData(tgHeroData* hero)
	{
		account = hero->account;
		tranid = hero->tranid;
		info = hero->info;
		gsid = hero->gsid;
	}
	account_type account;
	tran_id_type tranid;
	message::MsgHeroDataDB2GS info;
	u16 gsid;
};

struct tgVerify
{
	tgVerify(account_type acc, tran_id_type tran, u16 gs, const char* k)
	{
		account = acc;
		tranid = tran;
		gsid = gs;
		cd_key = k;
	}

	account_type account;
	tran_id_type tranid;
	u16 gsid;
	std::string cd_key;
};


DBQuestManager::DBQuestManager()
{
	_receive_cose_msg = false;
	//saveToClose(3);
}

DBQuestManager::~DBQuestManager()
{

}

void DBQuestManager::saveHeroMap(message::MsgSaveHeroDataGS2DB* msg)
{
	
}

void DBQuestManager::queryHeroInfo(account_type a, tran_id_type t, u16 gs)
{
	char sz_sql[256];
	sprintf(sz_sql, "select * from `character` where `account_id`=%llu;",a);
	gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbDoQueryHeroInfo, sz_sql, 0, new tgHeroData(a, t, gs), _QUERY_HERO_INFO_);
}


void DBQuestManager::onSaveToClose()
{
	gDBCharDatabase.setObjState(RunObject::_wait_stop_);
	Mylog::log_server(LOG_INFO, "close character mysql!");
}

void DBQuestManager::verifyToyCDKey(tran_id_type t, u16 gs, account_type acc, const char* cdkey)
{
	char sz_sql[256];
	sprintf(sz_sql, "select * from `character_toy` where `toy_cd_key`='%s';", cdkey);

	gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbDoQuerryToyVerify, sz_sql, 0, new tgVerify(acc, t, gs, cdkey), _QUERRY_VERIFY_TOY_CDKEY_);

}

void DBQuestManager::saveToClose(u16 gsid)
{
	if (_receive_cose_msg == false)
	{
		if (gEventMgr.hasEvent(this, EVENT_SAVE_TO_CLOSE) == false)
		{
			gDBCharDatabase.set_close_gs_id(gsid);
			gEventMgr.addEvent(this, &DBQuestManager::onSaveToClose, EVENT_SAVE_TO_CLOSE, _SAVE_TO_CLOSE_TIME_, 1, 0);
		}
		_receive_cose_msg = true;
		Mylog::log_server(LOG_INFO, "receive save all heroes msg!");
		message::MsgSaveAllHeroesDB2GS msg;
		gDBGameManager.sendMessage(&msg, 0, gsid);
	}
}


void DBQuestManager::saveSqlData(const char* sql)
{
	std::string str_excute = sql;
	gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbCallNothing, str_excute.c_str(), 0, NULL, _SAVE_HERO_INFO_);
}


void DBQuestManager::dbDoQuerryToyVerify(const SDBResult* r, const void* d, bool s)
{
	if (r != NULL)
	{
		tgVerify* pkParm = static_cast<tgVerify*>(const_cast<void*>(d));
		if (!pkParm)
		{
			return;
		}
		const SDBResult& result = *r;
		if (result.num_rows() != 0) 
		{
			const mysqlpp::Row row = result[0];
			message::MsgToyData entry;
			account_type acc = row["account"];
			entry.set_toy_cd_key(row["toy_cd_key"].c_str());
	
			message::MsgToyBaseData* entry_toy = entry.mutable_toy();
			entry_toy->set_toy_config_id(row["toy_config_id"]);
			entry_toy->set_toy_config_type(row["toy_config_type"]);
			entry_toy->set_toy_level(row["toy_level"]);
			entry.set_time_stamp(g_server_time);
			std::string verify_time = get_time(g_server_time);
			char sz_sql[512];
			sprintf(sz_sql, "update set `account`=%llu, `verify_time`='%s' where `toy_cd_key`='%s'",
				pkParm->account, verify_time.c_str(), pkParm->cd_key.c_str());
			gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbCallNothing, sz_sql, 0, NULL, _QUERRY_VERIFY_TOY_CDKEY_);
			message::MsgVerifyToyDB2GS msg;			
			msg.mutable_toy()->CopyFrom(entry);
			msg.set_account(acc);
			gDBGameManager.sendMessage(&msg, pkParm->tranid, pkParm->gsid);
			/*
			
			
			if (acc != 0)
			{
				message::MsgVerifyToyErrorDB2GS msg;
				msg.set_cdkey(pkParm->cd_key.c_str());
				msg.set_error(message::toy_cd_key_already_verify);
				gDBGameManager.sendMessage(&msg, pkParm->tranid, pkParm->gsid);			
			}
			else
			{
				time_t timep;			
				time(&timep);
				entry.set_toy_cd_key(row["toy_cd_key"].c_str());
				message::MsgToyBaseData* entry_toy = entry.mutable_toy();
				entry_toy->set_toy_config_id(row["toy_config_id"]);
				entry_toy->set_toy_config_type(row["toy_config_type"]);
				entry_toy->set_toy_level(1);
				entry.set_time_stamp(timep);
				
				std::string verify_time = get_time(timep);

				char sz_sql[512];
				sprintf(sz_sql, "update set `account_verify`=%llu, `verify_time`='%s' where `toy_cd_key`='%s'",
					pkParm->account, verify_time.c_str(), pkParm->cd_key.c_str());
				gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbCallNothing, sz_sql, 0, NULL, _QUERRY_VERIFY_TOY_CDKEY_);
				message::MsgVerifyToyDB2GS msg;
				msg.mutable_toy()->CopyFrom(entry);
				gDBGameManager.sendMessage(&msg, pkParm->tranid, pkParm->gsid);
			}
			*/
			
		}
		
	}
}



void DBQuestManager::dbDoQuerryHeroToys(const SDBResult* r, const void* d, bool s)
{
	if (r != NULL)
	{
		tgHeroData* pkParm = static_cast<tgHeroData*>(const_cast<void*>(d));
		if (!pkParm)
		{
			return;
		}
		const SDBResult& result = *r;
		for (int i = 0; i < result.num_rows(); i++)
		{
			const mysqlpp::Row row = result[i];
			message::MsgToyData* toy_entry = pkParm->info.add_toys();
			toy_entry->set_time_stamp(row["UNIX_TIMESTAMP(`verify_time`)"]);
			toy_entry->set_toy_cd_key(row["toy_cd_key"].c_str());
			message::MsgToyBaseData* base_toy = toy_entry->mutable_toy();
			base_toy->set_toy_config_id(row["toy_config_id"]);
			base_toy->set_toy_config_type(row["toy_config_type"]);
			base_toy->set_toy_level(row["toy_level"]);
		}
		gDBGameManager.sendMessage(&pkParm->info, pkParm->tranid, pkParm->gsid);
	}
}

void DBQuestManager::dbDoQueryHeroEquips(const SDBResult* r, const void* d, bool s)
{
	if (r != NULL)
	{
		tgHeroData* pkParm = static_cast<tgHeroData*>(const_cast<void*>(d));
		if (!pkParm)
		{
			return;
		}
		const SDBResult& result = *r;
		for (int i = 0; i < result.num_rows(); i++)
		{
			const mysqlpp::Row row = result[i];
			message::MsgEquipData* eqiup = pkParm->info.add_equips();
			eqiup->set_id(row["equip_id"]);
			eqiup->set_equip_id(row["equip_config_id"]);
			eqiup->set_equipped((bool)row["equipped"]);
			eqiup->set_level(row["level"]);
			eqiup->set_client_save_flag(row["client_save_flag"]);
			eqiup->set_count(row["equip_count"]);
		}
		char sztemp[256];
		sprintf(sztemp, "select * from `character_toy` where `account`=%llu;", pkParm->account);
		gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbDoQuerryHeroToys, sztemp, 0, new tgHeroData(pkParm), _QUERY_HERO_INFO_);
	}

}


void DBQuestManager::dbDoQueryHeroInfo(const SDBResult* r, const void* d, bool s)
{
	tgHeroData* pkParm = static_cast<tgHeroData*>(const_cast<void*>(d));
	if (!pkParm)
	{
		return;
	}

	bool need_create = true;
	if (r != NULL)
	{
		const SDBResult& result = *r;
		if (result.num_rows() > 0)
		{
			const mysqlpp::Row row = result[0];
			account_type acc = row["account_id"];
			pkParm->info.set_account(acc);
			message::MsgHeroData* data = pkParm->info.mutable_data();
			data->set_level(row["level"]);
			std::string name_temp = row["name"].c_str();
			data->set_name(name_temp.c_str());
			data->set_action_point(row["action_point"]);
			data->set_gold(row["gold"]);
			data->set_diamand(row["diamand"]);
			data->set_account(acc);
			std::string sql_suits_name = row["suits_name"].c_str();
			std::vector<std::string> vc_str;
			std::vector<std::string> vc_suit_info;
			
			SplitStringA(sql_suits_name, ":", vc_str);
			std::vector<std::string>::iterator it_vc_str = vc_str.begin();
			for (; it_vc_str != vc_str.end(); ++ it_vc_str)
			{
				std::string str_temp = (*it_vc_str);
				SplitStringA(str_temp, ",", vc_suit_info);
				if (vc_suit_info.size() >= 2)
				{
					int id_suits = 0;
					std::string id_suits_name;
					
					if (isIntger(vc_suit_info[0].c_str()) == true)
					{
						id_suits = atoi(vc_suit_info[0].c_str());
						id_suits_name = vc_suit_info[1].c_str();
						message::MsgSuitData* suit_data = data->add_suits();
						suit_data->set_suit_id(id_suits);
						suit_data->set_suit_name(id_suits_name.c_str());												
						int siez_temp = vc_suit_info.size();
						for (int i = 2; i < siez_temp; i ++)
						{
#ifdef WIN32
							u64 equip_id_temp = _atoi64(vc_suit_info[0].c_str());
#elif  WIN64
							u64 equip_id_temp = _atoi64(vc_suit_info[0].c_str());
#else
							u64 equip_id_temp = strtol(vc_suit_info[0].c_str(), NULL, 10);
#endif // WIN32							
							suit_data->add_equip_ids(equip_id_temp);
						}
					}
				}
			}
			need_create = false;
			char sztemp[256];
			sprintf(sztemp, "select * from `character_equip` where `account_id`=%llu;", acc);
			gDBCharDatabase.addSQueryTask(this, &DBQuestManager::dbDoQueryHeroEquips, sztemp, 0, new tgHeroData(pkParm), _QUERY_HERO_INFO_);
		}
	}

	if (need_create)
	{
		message::MsgNeedCreateHeroDB2GS msg;
		gDBGameManager.sendMessage(&msg, pkParm->tranid, pkParm->gsid);
	}
}
