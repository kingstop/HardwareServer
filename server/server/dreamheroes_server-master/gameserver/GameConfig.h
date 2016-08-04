#pragma once
#include <vector>
#include <map>

struct HeroEquipConfig
{
	int equip_config_id;
	int equip_level;
	int equip_conut;
};



struct HeroConfig
{
	int gold;
	int diamond;
	int level;
	std::vector<HeroEquipConfig> equips;
	std::map<int, std::string> suits_name;

};

struct HeroEquipLevelUpConfig
{
	int toy_config_count;
	int toy_config_diamand;
	int toy_config_gold;
};

typedef std::map<int, HeroEquipLevelUpConfig> LEVELEQUIPCONFIGS;
typedef std::map<int, LEVELEQUIPCONFIGS> EQUIPSLEVELUPCONFIGS;


class GameConfig
{
public:
	GameConfig();
	virtual ~GameConfig();
	void Load(DBQuery* p);
	void set_generate_equip_id(u64 equpid);
	u64 generateEquipID();
	const HeroEquipLevelUpConfig* getEquipLevelUpConfig(int config_id, int config_level);
public:
	HeroConfig getHeroConfig();
protected:
	HeroConfig _hero_config;
	EQUIPSLEVELUPCONFIGS _equips_level_up_configs;
	u64 _generate_id;
};

