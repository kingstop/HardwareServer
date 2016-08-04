#include "stdafx.h"
#include "GameConfig.h"


GameConfig::GameConfig()
{
	_generate_id = 0;
}


GameConfig::~GameConfig()
{
}


HeroConfig GameConfig::getHeroConfig()
{
	return _hero_config;
}

u64 GameConfig::generateEquipID()
{
	_generate_id++;
	return _generate_id;
}

const HeroEquipLevelUpConfig* GameConfig::getEquipLevelUpConfig(int config_id, int config_level)
{

	EQUIPSLEVELUPCONFIGS::iterator it = _equips_level_up_configs.find(config_id);
	if (it != _equips_level_up_configs.end())
	{
		LEVELEQUIPCONFIGS::iterator it_level = it->second.find(config_level);
		if (it_level != it->second.end())
		{
			return &it_level->second;
		}			
	}
	return NULL;
}

void GameConfig::set_generate_equip_id(u64 equpid)
{
	_generate_id = equpid;
}

void GameConfig::Load(DBQuery* p)
{

	if (p)
	{
		DBQuery& query = *p;
		query << "select * from `hero_info`;";
		SDBResult sResult = query.store();
		if (sResult.num_rows() > 0)
		{
			DBRow& row = sResult[0];
			_hero_config.gold = row["hero_gold"];
			_hero_config.diamond = row["hero_diamond"];
			_hero_config.level = row["hero_level"];
		}

		query.reset();
		sResult.clear();
		query << "select * from `hero_equip`;";
		sResult = query.store();
		for (int i = 0; i < sResult.num_rows(); i++)
		{
			DBRow& row = sResult[i];
			HeroEquipConfig entry;
			entry.equip_config_id = row["equip_config_id"];
			entry.equip_conut = row["equip_count"];
			entry.equip_level = row["equip_level"];
			_hero_config.equips.push_back(entry);
		}

		sResult.clear();
		query.reset();
		query << "select * from `suit_name_config`";
		sResult = query.store();
		for (int i = 0; i < sResult.num_rows(); i++)
		{
			DBRow& row = sResult[i];
			_hero_config.suits_name[row["suit_id"]] = row["suit_name"].c_str();
		}

		sResult.clear();
		query.reset();
		query << "select * from `hero_equip_level_config`;";
		sResult = query.store();
		for (int i = 0; i < sResult.num_rows(); i++)
		{
			DBRow& row = sResult[i];
			int config_id = row["toy_config_id"];
			int config_type = row["toy_config_type"];
			int config_level = row["toy_config_level"];
			EQUIPSLEVELUPCONFIGS::iterator it = _equips_level_up_configs.find(config_id);

			if (it == _equips_level_up_configs.end())
			{
				LEVELEQUIPCONFIGS temp_map;
				_equips_level_up_configs[config_id] = temp_map;
			}


			LEVELEQUIPCONFIGS::iterator it_level = _equips_level_up_configs[config_id].find(config_level);
			if (it_level == _equips_level_up_configs[config_id].end())
			{
				HeroEquipLevelUpConfig config_level_up;
				config_level_up.toy_config_count = row["toy_config_count"];
				config_level_up.toy_config_diamand = row["toy_config_diamand"];
				config_level_up.toy_config_gold = row["toy_config_gold"];
				_equips_level_up_configs[config_id][config_level] = config_level_up;
			}
		}
		sResult.clear();
		query.reset();
	}	

}