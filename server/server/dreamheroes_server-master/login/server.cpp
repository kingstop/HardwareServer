#include "stdafx.h"


LoginServer::LoginServer():m_EventHold(WORLD_INSTANCE)
{

}
LoginServer::~LoginServer()
{

}
bool LoginServer::init()
{
    ServerFrame::init();
    DataBaseConfig dbconfig;
    if (!ServerFrame::loadDBConfig(dbconfig, CENTER_DATABASE_CONFIG))
    {
        Mylog::log_server( LOG_ERROR, "load database config failed !");
        return false;
    }
   
    if (!Database::addBlockTask(dbconfig, this, &LoginServer::initDataFromDatabase, NULL))
    {
        Mylog::log_server( LOG_ERROR, "Init Server from DataBase Error!");
        return false;
    }

    if(!gLGCenterDB.initDatabase( dbconfig))
    {
        Mylog::log_server( LOG_ERROR, "init database failed!");
        return false;
    }
    Mylog::log_server( LOG_INFO, "Start Login Server v[%s] Success!", gLGVersion.toString().c_str());
    return true;
}

bool LoginServer::onKey()
{
    return true;
}

void LoginServer::runOnce(u32 nDiff)
{
    net_global::update_net_service();
	gLGServerTime = time(NULL);
    gLGCenterDB.run(nDiff);
	gLGUserServer.run_no_wait();
	gLGGateServer.run_no_wait();
    m_EventHold.update(nDiff);

    if (nDiff > 800)
    {
        Mylog::log_server(LOG_WARNING, "server delay [%u]", nDiff);
    }
}
void LoginServer::shutDown()
{
	gLGUserServer.stop();
	gLGGateServer.stop();
    gLGCenterDB.shutDown();
}

void LoginServer::memoryLog() 
{

}
void LoginServer::setStop()
{
	_wait_stop = true;
}
void LoginServer::checkStop()
{
	_stop = true;
}
bool LoginServer::initDataFromDatabase(DBQuery* p, const void* data)
{
	if (!p)
	{ return false;}

	net_info user_net;
	DBQuery& query = *p;
	DBQParms parms;

	parms << _LOGIN_LISTEN_USER_;
	query << "SELECT * FROM `net_config` WHERE `Category` = %0";
	query.parse();

	if (!ServerFrame::getNetConfig(user_net, query.use(parms)))
	{ 
		Mylog::log_server( LOG_ERROR, "Init login server net config failed !");
		return false;
	}

	if(!gLGUserServer.create( user_net._port, user_net._max_connect, user_net._thread_count))
	{
		Mylog::log_server( LOG_ERROR, "UDPSERVER Init NetConfig failed!");
		return false;
	}
	
	Mylog::log_server( LOG_INFO, "UserListen begin lister port[%d], MaxConnect[%d], ThreadCount[%d]!", user_net._port, user_net._max_connect, user_net._thread_count);
	
	parms.clear();
	query.reset();
	parms << _LOGIN_LISTEN_GATE_;
	query << "SELECT * FROM `net_config` WHERE `Category` = %0";
	query.parse();

	if (!ServerFrame::getNetConfig(user_net, query.use(parms)))
	{ 
		Mylog::log_server( LOG_ERROR, "Init gate server net config failed !");
		return false;
	}

	if (!gLGGateServer.create( user_net._port, user_net._max_connect, user_net._thread_count))
	{
		Mylog::log_server( LOG_ERROR, "GateListen Init NetConfig failed!");
		return false;
	}
	Mylog::log_server( LOG_INFO, "GateListen begin lister port[%d], MaxConnect[%d], ThreadCount[%d]!", user_net._port, user_net._max_connect, user_net._thread_count);
    return true;
}
