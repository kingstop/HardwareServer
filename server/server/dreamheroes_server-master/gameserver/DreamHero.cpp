#include "stdafx.h"
#include "DreamHero.h"
#include "session.h"
#define _SAVE_PLAYER_TIME_  (10 * _TIME_SECOND_MSEL_)
DreamHero::DreamHero()
{
	_info.set_action_point(0);
	_info.set_diamand(0);
	_info.set_gold(0);
	_info.set_level(1);
	_info.set_name("test");
	_online = false;
}


DreamHero::~DreamHero()
{
	StopSave();
}


message::MsgHeroData DreamHero::get_info()
{
	return _info;
}


u64 DreamHero::get_account()
{
	return _account;
}

void DreamHero::set_account(u64 account)
{
	_account = account;
	_info.set_account(_account);
}

void DreamHero::set_info(const message::MsgHeroDataDB2GS* info)
{
	///std::string cd_key_temp = info->cd
	info->account();
	_info.CopyFrom(info->data());
	for (int i = 0; i < info->toys_size(); i ++)
	{
		message::MsgToyData entry;
		const message::MsgToyData temp = info->toys(i);
		entry.CopyFrom(temp);
		_hero_toys.insert(HEROTOYS::value_type(temp.toy_cd_key(), entry));
	}

	for (int i = 0; i < info->equips_size(); i ++)
	{
		message::MsgEquipData entry;
		const message::MsgEquipData temp = info->equips(i);
		entry.CopyFrom(temp);
		_hero_equips.insert(HEROEQUIPS::value_type(entry.id(), entry));
	}
	
}

void DreamHero::ModifySuit(const message::C2SModifySuitReq* msg)
{
	const message::MsgSuitData suitData = msg->data();
	bool modify_temp = false;
	int suits_size_temp = _info.suits_size();
	for (int i = 0; i < suits_size_temp; i ++)
	{
		if (_info.suits(i).suit_id() == suitData.suit_id())
		{
			_info.mutable_suits(i)->CopyFrom(suitData);
			modify_temp = true;
		}		
	}	
	if (modify_temp == false)
	{
		message::MsgSuitData* suitDataTemp = _info.add_suits();
		suitDataTemp->CopyFrom(suitData);		
	}

	message::S2CModifySuitACK msgSuitACK;
	msgSuitACK.set_suit_id(suitData.suit_id());
	msgSuitACK.set_error(message::no_error);
	sendPBMessage(&msgSuitACK);	
}

void DreamHero::DelSuit(const message::C2SDelSuitReq* msg)
{
	int suit_id_temp =  msg->suit_id();
	int suits_size_temp = _info.suits_size();
	google::protobuf::RepeatedPtrField< ::message::MsgSuitData >::const_iterator it = _info.suits().begin();
	message::HeroErrorCode error_temp = message::server_error_no_suit_id;
	for (; it != _info.suits().end(); it++)
	{
		if (it->suit_id() == suit_id_temp)
		{
			_info.mutable_suits()->erase(it);
			error_temp = message::no_error;
		}
	}

	message::S2CDelSuitACK msg_ACK;
	msg_ACK.set_suit_id(suit_id_temp);
	msg_ACK.set_error(error_temp);
	sendPBMessage(&msg_ACK);	
}

void DreamHero::VerifyToyCDKey(message::C2SVerifyToyCDKeyReq* msg)
{
	std::string cd_key_temp = msg->cd_key();
	HEROTOYS::iterator it = _hero_toys.find(cd_key_temp.c_str());
	if (it != _hero_toys.end())
	{
		message::S2CVerifyToyCDKeyErrorACK msg_ACK;
		msg_ACK.set_cd_key(cd_key_temp.c_str());
		msg_ACK.set_error(message::toy_cd_key_have_been_in_used);
		sendPBMessage(&msg_ACK);
	}
	else
	{
		message::MsgVerifyToyGS2DB msg_db;
		msg_db.set_cdkey(cd_key_temp.c_str());
		gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
	}
}

void DreamHero::EquipLevelUp(message::S2CEquipLevelUpReq* msg)
{
	u64 equip_id = msg->equip_id();
	message::HeroErrorCode cur_error = message::no_error;

	HEROEQUIPS::iterator it_equip = _hero_equips.find(equip_id);
	if (it_equip != _hero_equips.end())
	{
		int level = it_equip->second.level();
		int config_id = it_equip->second.equip_id();
		int count = it_equip->second.count();
		level += 1;
		const HeroEquipLevelUpConfig* equip_level_config = gGameConfig.getEquipLevelUpConfig(config_id, level);
		if (equip_level_config != NULL)
		{
			if (count < equip_level_config->toy_config_count)
			{
				cur_error = message::equip_level_up_not_enough_equip;
			}
			else
			{
				if (_info.gold() <  equip_level_config->toy_config_gold)
				{
					cur_error = message::equip_level_up_not_enough_gold;
				}
				else
				{
					if (_info.diamand() < equip_level_config->toy_config_diamand)
					{
						cur_error = message::equip_level_up_not_enough_diamand;
					}
					else
					{
						int gold_temp = _info.gold() - equip_level_config->toy_config_gold;
						int diamand_temp = _info.diamand() - equip_level_config->toy_config_diamand;
						int count_temp = count - equip_level_config->toy_config_count;
						_info.set_gold(gold_temp);
						_info.set_diamand(diamand_temp);
						it_equip->second.set_count(count_temp);
						it_equip->second.set_level(level);
						message::S2CEquipLevelUpACK msg_send;
						msg_send.set_count(count_temp);
						msg_send.set_gold(gold_temp);
						msg_send.set_level(level);
						msg_send.set_diamand(diamand_temp);
						sendPBMessage(&msg_send);
					}
				}
			}
		}

	}
	else
	{
		cur_error = message::equip_level_up_not_found_equip;
	}
	if (cur_error != message::no_error)
	{
		message::S2CEquipLevelUpErrorACK error_ACK;
		error_ACK.set_equip_id(equip_id);
		error_ACK.set_error(cur_error);
		sendPBMessage(&error_ACK);
	}
}

void DreamHero::VerifyToy(message::MsgVerifyToyDB2GS* msg)
{
	std::string cd_key_temp = msg->toy().toy_cd_key();
	account_type acc_temp = msg->account();
	DreamHero* temp_hero = gDreamHeroManager.GetHero(acc_temp);
	if (temp_hero)
	{
		temp_hero->RemoveToyByCDKey(cd_key_temp.c_str());
		gDreamHeroManager.modifyToyAccount(_account, cd_key_temp.c_str());
	}
	
	HEROTOYS::iterator it = _hero_toys.find(cd_key_temp.c_str());
	if (it != _hero_toys.end())
	{
		Mylog::log_server(LOG_WARNING, "toy cd key[%s] is already used", cd_key_temp.c_str());
	}
	else
	{

		message::MsgToyData ToyData;
		ToyData.CopyFrom(msg->toy());
		_hero_toys.insert(HEROTOYS::value_type(cd_key_temp, ToyData));
		message::S2CVerifyToyCDKeyACK msg_ACK;
		msg_ACK.mutable_toydata()->CopyFrom(ToyData);
		sendPBMessage(&msg_ACK);
		/*
		account_type acc = gDreamHeroManager.getToyHero(cd_key_temp.c_str());
		if (acc != 0)
		{
			Mylog::log_server(LOG_WARNING, "toy cd key[%s] is already used", cd_key_temp.c_str());
		}
		else
		{

		}
		*/
	}
}

void DreamHero::VerifyToy(message::MsgVerifyToyErrorDB2GS* msg)
{
	message::S2CVerifyToyCDKeyErrorACK msg_ACK;
	msg_ACK.set_cd_key(msg->cdkey().c_str());
	msg_ACK.set_error(msg->error());
	sendPBMessage(&msg_ACK);
}


void DreamHero::modify_suit(int suit_config, const char* szname)
{
	bool have_suit_id = false;
	int suit_size_temp = _info.suits_size();
	for (int i = 0; i < suit_size_temp; i ++)
	{
		message::MsgSuitData* suitData = _info.mutable_suits(i);
		if (suitData->suit_id() == suit_config)
		{
			have_suit_id = true;
			suitData->set_suit_name(szname);
			message::S2CModifySuitNameACK msg;
			msg.set_suit_id(suit_config);
			msg.set_suit_name(szname);
			sendPBMessage(&msg);
			break;
		}		
	}

	if (have_suit_id == false)
	{
		message::S2CModifySuitErrorACK msg;
		msg.set_suit_id(suit_config);
		msg.set_error(message::server_error_no_suit_id);
		sendPBMessage(&msg);
	}
}

void DreamHero::set_session(Session* session)
{
	_session = session;
}

Session* DreamHero::get_session()
{
	return _session;
}


std::vector<int> MapVersionFormat(std::string cur_version)
{
	std::vector<std::string> vcstr;
	std::vector<int> vcret;
	SplitStringA(cur_version, ".", vcstr);
	for (int i = 0; i < vcstr.size(); i ++)
	{
		if (isIntger(vcstr[i].c_str()))
		{
			vcret.push_back(atoi(vcstr[i].c_str()));
		}
		else
		{
			break;
		}
	}
	return vcret;
}

void DreamHero::set_level(int level)
{
	_info.set_level(level);
}

void DreamHero::StartSave()
{
	if (gEventMgr.hasEvent(this, EVENT_SAVE_PLAYER_DATA_) == false)
	{
		gEventMgr.addEvent(this, &DreamHero::SaveHero, EVENT_SAVE_PLAYER_DATA_, _SAVE_PLAYER_TIME_, -1, 0);
	}
}

void DreamHero::StopSave()
{
	if (gEventMgr.hasEvent(this, EVENT_SAVE_PLAYER_DATA_) == true)
	{
		gEventMgr.removeEvents(this, EVENT_SAVE_PLAYER_DATA_);
	}
}



void DreamHero::set_online(bool online)
{
	_online = online;
}

bool DreamHero::is_online()
{
	return _online;
}


void DreamHero::RemoveToyByCDKey(const char* cd_key)
{
	HEROTOYS::iterator it = _hero_toys.find(cd_key);
	if (it != _hero_toys.end())
	{
		_hero_toys.erase(it);
	}	
}

void DreamHero::SendClientInit()
{
	StartSave();
	message::S2CMsgHeroInit msg;
	msg.mutable_hero()->CopyFrom(_info);
	int max_count = 40;
	int current_count = 0;
	message::S2CMsgEquipInit equip_msg;
	message::S2CMsgHeroEquip* equip_temp = NULL;
	equip_msg.mutable_equip();
	HEROEQUIPS::iterator it_temp = _hero_equips.begin();
	int max_count_equip = _hero_equips.size();
	int current_equip_count = 0;
	for (current_equip_count = 1; it_temp != _hero_equips.end(); ++ it_temp, current_count ++, current_equip_count ++)
	{
		if (current_count == 0)
		{
			equip_temp = equip_msg.mutable_equip();			
		}
		message::MsgEquipData* equip_data = equip_temp->add_equips();
		equip_data->CopyFrom(it_temp->second);
		if (current_equip_count == 1)
		{
			equip_temp->set_begin(true);
		}
		if (current_equip_count == max_count_equip)
		{
			equip_temp->set_end(true);
		}

		if (current_count >= max_count)
		{
			sendPBMessage(&equip_msg);
			equip_msg.clear_equip();
		}				
	}

	if (equip_msg.mutable_equip()->equips_size() > 0)
	{
		sendPBMessage(&equip_msg);
	}

	message::S2CMsgToyInit msg_toy_init;
	HEROTOYS::iterator it_toy = _hero_toys.begin();
	for (; it_toy != _hero_toys.end(); ++ it_toy)
	{
		message::MsgToyBaseData* data = msg_toy_init.add_toys();
		data->CopyFrom(it_toy->second.toy());				
	}
	sendPBMessage(&msg_toy_init);

	sendPBMessage(&msg);
	_online = true;
}


void DreamHero::LoadFromConfig()
{
	_info.set_diamand(gGameConfig.getHeroConfig().diamond);
	_info.set_level(gGameConfig.getHeroConfig().level);
	_info.set_gold(gGameConfig.getHeroConfig().gold);
	if (gGameConfig.getHeroConfig().equips.empty() == false)
	{
		int size_temp = gGameConfig.getHeroConfig().equips.size();
		for (int i = 0; i < size_temp; i ++)
		{
			HeroEquipConfig entry_equip_config = gGameConfig.getHeroConfig().equips[i];
			message::MsgEquipData entry;
			entry.set_id(gGameConfig.generateEquipID());
			entry.set_equipped(true);
			entry.set_client_save_flag(0);
			entry.set_level(entry_equip_config.equip_level);
			entry.set_equip_id(entry_equip_config.equip_config_id);
			entry.set_count(entry_equip_config.equip_conut);
			_hero_equips[entry.id()] = entry;
		}
	}

	HeroConfig entry_config = gGameConfig.getHeroConfig();
	std::map<int, std::string>::iterator it_temp = entry_config.suits_name.begin();
	std::map<int, std::string>::iterator it_end = entry_config.suits_name.end();
	while (it_temp != it_end)
	{
		message::MsgSuitData* data = _info.add_suits();
		int temp = it_temp->first;
		data->set_suit_id(temp);
		data->set_suit_name(it_temp->second.c_str());
		++it_temp;
	}
}

void DreamHero::SaveHero()
{
	std::string sql_temp;
	char temp[2048];
	int suits_size_temp = _info.suits_size();
	char suit_temp[256];
	std::string suits_sql;
	for (int i = 0; i < suits_size_temp; i ++)
	{
		if (i != 0)
		{
			suits_sql += ":";
		}
		const message::MsgSuitData temp_data = _info.suits(i);

		sprintf(suit_temp, "%d,%s", temp_data.suit_id(), temp_data.suit_name().c_str());
		suits_sql += suit_temp;
		for (int i = 0; i < temp_data.equip_ids_size(); i ++)
		{
			suits_sql += ",";
			u64 equip_id = temp_data.equip_ids(i);
#ifdef WIN32
			sprintf(suit_temp, "%llu", equip_id);
#else
			sprintf(suit_temp, "%llu", equip_id);
#endif
			suits_sql += suit_temp;
		}		
	}

#ifdef WIN32
	sprintf(temp, "replace into `character`(`account_id`, `level`, `name`,`action_point`, `diamand`, `gold`, `suits_name`) values(%llu, %d, '%s', %d, %d, %d, '%s')",
		_info.account(), _info.level(), _info.name().c_str(), _info.action_point(), _info.diamand(), _info.gold(), suits_sql.c_str());

#else
	sprintf(temp, "replace into `character`(`account_id`, `level`, `name`,`action_point`, `diamand`, `gold`, `suits_name`) values(%lu, %d, '%s', %d, %d, %d, '%s')",
		_info.account(), _info.level(), _info.name().c_str(), _info.action_point(), _info.diamand(), _info.gold(), suits_sql.c_str());

#endif // WIN32
	sql_temp = temp;
	message::MsgSaveDataGS2DB msg_db;
	msg_db.set_sql(sql_temp.c_str());
	sql_temp.clear();
 
	gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
	//gGSDBClient.parseQueryChar()
	if (_hero_equips.size() != 0)
	{
		std::string sql_item = "replace into `character_equip`(`equip_id`, `account_id`, `equip_config_id`, `level`, `equipped`, `client_save_flag`,`equip_count`) values";
		HEROEQUIPS::iterator it = _hero_equips.begin();
		int item_count = 0;
		for (; it != _hero_equips.end(); ++ it, item_count ++)
		{
			if (item_count >= 20)
			{
				msg_db.clear_sql();
				msg_db.set_sql(sql_temp.c_str());
				gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
				item_count = 0;
			}

			if (item_count == 0)
			{
				sql_temp += sql_item;
			}
			else
			{
				sql_temp += ",";
			}
			message::MsgEquipData entry = it->second; 
#ifdef WIN32
			sprintf(temp, "(%llu, %llu, %d, %d, %d, %d, %d)", entry.id(), _info.account(), entry.equip_id(), entry.level(), (int)entry.equipped(), entry.client_save_flag(), entry.count());
#else
			sprintf(temp, "(%lu, %lu, %d, %d, %d, %d, %d)", entry.id(), _info.account(), entry.equip_id(), entry.level(), (int)entry.equipped(), entry.client_save_flag(), entry.count());
#endif // 
			sql_temp += temp;

		}

		if (sql_temp.empty() == false)
		{
			msg_db.clear_sql();
			msg_db.set_sql(sql_temp.c_str());
			gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
		}
	}

	if (_hero_toys.size() != 0)
	{
		std::string sql_head = "replace into `character_toy`(`toy_cd_key`, `account`, `toy_config_id`, `toy_config_type`, `toy_level`, `verify_time`) values";
		std::string sql_excute = "";
		HEROTOYS::iterator it_toy = _hero_toys.begin();
		int toy_save_count = 0;
		for (; it_toy != _hero_toys.end(); ++it_toy, toy_save_count++)
		{
			if (toy_save_count > 20)
			{
				toy_save_count = 0;
				msg_db.clear_sql();
				msg_db.set_sql(sql_excute.c_str());
				gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
				sql_excute.clear();
			}

			if (toy_save_count == 0)
			{
				sql_excute += sql_head;
			}
			else
			{
				sql_excute += ",";
			}

			char sztemp[512];
			std::string verify_time = get_time(it_toy->second.time_stamp());
			sprintf(sztemp, "('%s', '%llu', %d, %d, %d, '%s')", it_toy->second.toy_cd_key().c_str(), _account,
				it_toy->second.toy().toy_config_id(), it_toy->second.toy().toy_config_type(), it_toy->second.toy().toy_level(), verify_time.c_str());
			sql_excute += sztemp;
		}

		if (sql_excute.empty() == false)
		{
			msg_db.clear_sql();
			msg_db.set_sql(sql_excute.c_str());
			gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());
		}
	}
}

void DreamHero::set_name(const char* name)
{
	_info.set_name(name);
}

int DreamHero::get_level()
{
	return _info.level();
}
const char* DreamHero::get_name()
{
	return _info.name().c_str();
}




void DreamHero::sendPBMessage(google::protobuf::Message* p)
{
	if (_session)
	{
		_session->sendPBMessage(p);
	}
	
}
