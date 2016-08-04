#include "stdafx.h"

GameServer::GameServer()
    :m_EventHold(WORLD_INSTANCE)
{
}

bool GameServer::init()
{
    ServerFrame::init();
    DataBaseConfig dbconfig;

    if (!ServerFrame::loadDBConfig(dbconfig, CENTER_DATABASE_CONFIG))
    {
		//printf("game db server init  failed \n" );
        Mylog::log_server( LOG_ERROR, "load database config failed !");
        return false;
    }


    if (!Database::addBlockTask(dbconfig, this, &GameServer::initDataFromDatabase, NULL))
    {
		//printf("init from Database server init  failed \n" );
        Mylog::log_server( LOG_ERROR, "Init Server from DataBase Error!");
        return false;
    }

	if (gCenterDataBase.initDatabase(dbconfig) == false)
	{
		Mylog::log_server( LOG_ERROR, "init center db server init  failed \n" );
		return false;
	}

    Mylog::log_server( LOG_INFO, "Start Game Server v:[%u] Success!", gGSVersion.toString().c_str());
    return true;
}



bool GameServer::onKey()
{

    return true;

}


void GameServer::runOnce(u32 nDiff)
{
    net_global::update_net_service();
	gGSServerTime = time(NULL);
    gGSGateServer.run_no_wait();
    gGSDBClient.run_no_wait();
    m_EventHold.update(nDiff);
    static u32 curtime = 0;
    curtime += nDiff;
    if (curtime > 15000)
    {
        curtime = 0;
    }

    if (nDiff > 800)
    {
        Mylog::log_server(LOG_WARNING, "server delay [%u]", nDiff);
    }
}

void GameServer::shutDown()
{
    gGSDBClient.close();
    gGSGateServer.stop();
}



void GameServer::memoryLog() 
{

}

void GameServer::setStop()
{
	gDreamHeroManager.SaveDreamHeroes();
	Mylog::log_server(LOG_INFO, "prepare to stop game server!");
	
}

void GameServer::checkStop()
{
	if (gDreamHeroManager.is_save_all_heroes_ok())
	{
		gDreamHeroManager.SaveDreamHeroes();
		Mylog::log_server(LOG_INFO, "game server is stopping!");
		shutDown();
		_stop = true;
	}
}

bool GameServer::initDataFromDatabase(DBQuery* p, const void* data)
{
	if (!p)
	{ 
		return false;
	}

 	net_info user_net;
 	DBQuery& query = *p;
 	DBQParms parms;
 	parms << _GAME_LISTEN_GATE_;
	parms << m_service_conf.id;
 	query << "SELECT * FROM `net_config` WHERE `Category` = %0 AND `ServerId` = %1";
 	query.parse();

 	if (!ServerFrame::getNetConfig(user_net, query.use(parms)))
 	{ 
 		Mylog::log_server( LOG_ERROR, "Init game server net config failed !");
 		return false;
 	}

 	if(!gGSGateServer.create( user_net._port, user_net._max_connect, user_net._thread_count))
 	{
 		Mylog::log_server( LOG_ERROR, "game Init NetConfig failed!");
 		return false;
 	}

 	Mylog::log_server( LOG_INFO, "ListenGate begin lister port[%d], MaxConnect[%d], ThreadCount[%d]!", user_net._port, user_net._max_connect, user_net._thread_count);
	parms.clear();
	query.reset();
	parms <<  m_service_conf.id;
 	parms.clear();
 	query.reset();
 	parms << _DB_LISNEN_GAME_;
 	query << "SELECT * FROM `net_config` WHERE `Category` = %0";
 	query.parse();
 	if (!ServerFrame::getNetConfig(user_net, query.use(parms)))
 	{ 
 		Mylog::log_server( LOG_ERROR, "Init gate server net config failed !");
 		return false;
 	}
 	gGSDBClient.connect(user_net._ip.c_str(), user_net._port);
 	Mylog::log_server( LOG_INFO, "connect db  port[%d], ip[%s]!", user_net._port, user_net._ip.c_str());
	parms.clear();
	query.reset();
	parms << _FU_WORLD_DATABASE_;
	query << "SELECT * FROM `database_config` WHERE `Category` = %0";
	query.parse();
	if (!ServerFrame::getDatabaseConfig(m_world_db_conf, query.store(parms)))
	{ 
		Mylog::log_server( LOG_ERROR, "load world db config failed !");
		return false;
	}
	if (gWorldDatabase.initDatabase(m_world_db_conf) == false)
	{
		Mylog::log_server( LOG_ERROR, "init world db failed !");
		return false;
	}

	if (!Database::addBlockTask(m_world_db_conf, this, &GameServer::initDataFromWorldDB, NULL))
	{
		Mylog::log_server( LOG_ERROR, "Init world server from database error!");
		return false;
	}

	parms.clear();
	query.reset();
	parms << _FU_CHARACTER_DATABASE_;
	query << "SELECT * FROM `database_config` WHERE `Category` = %0";
	query.parse();
	if (!ServerFrame::getDatabaseConfig(m_character_conf, query.store(parms)))
	{ 
		Mylog::log_server( LOG_ERROR, "load character db config failed !");
		return false;
	}

	if (!Database::addBlockTask(m_character_conf, this, &GameServer::initDataFromCharacterDB, NULL))
	{
		Mylog::log_server(LOG_ERROR, "Init character server from database error");
		return false;
	}

	if (!gDBCharDatabase.initDatabase(m_character_conf))
	{
		Mylog::log_server(LOG_ERROR, "start character db service failed!");
	}    
    return true;
}

bool GameServer::initDataFromCharacterDB(DBQuery* p, const void* data)
{
	if (!p)
	{
		return false;
	}

	p->reset();
	DBQuery& query = *p;
	query << "select MAX(`equip_id`) from `character_equip`;";
	SDBResult sResult = query.store();
	u64 generate_id = 0;
	if (sResult.num_rows() > 0)
	{
		DBRow& row = sResult[0];
		if (row["MAX(`equip_id`)"].is_null())
		{
		}
		else
		{
			generate_id = row["MAX(`equip_id`)"];
		}
	}
	gGameConfig.set_generate_equip_id(generate_id);
	return true;
}



bool GameServer::initDataFromWorldDB(DBQuery* p, const void* data)
{
	if (!p)
	{ 
		return false;
	}
	gGameConfig.Load(p);
	return true;
}

