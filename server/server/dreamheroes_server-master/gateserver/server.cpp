#include "stdafx.h"

GateServer::GateServer():m_EventHold(WORLD_INSTANCE)
{

}
bool GateServer::init()
{
	ServerFrame::init();
	DataBaseConfig dbconfig;
	if (!ServerFrame::loadDBConfig(dbconfig, CENTER_DATABASE_CONFIG))
	{
		Mylog::log_server( LOG_ERROR, "load database config failed !");
		return false;
	}

	if (!Database::addBlockTask(dbconfig, this, &GateServer::initDataFromDatabase, NULL))
	{
		Mylog::log_server( LOG_ERROR, "Init Server from DataBase Error!");
		return false;
	}

	Mylog::log_server( LOG_INFO, "Start Gate Server v:[%u] Success!", gGTVersion.toString().c_str());
	return true;
}

bool GateServer::onKey()
{
	return true;
}

void GateServer::runOnce(u32 nDiff)
{
    net_global::update_net_service();
	gGTServerTime = time(NULL);
	gGTLoginClient.run_no_wait();
    gGTUserServer.run_no_wait();
    gGTGameMgr.update(nDiff);
    m_EventHold.update(nDiff);

    if (nDiff > 800)
    {
        Mylog::log_server(LOG_WARNING, "gate delay [%u]", nDiff);
    }
}
void GateServer::shutDown()
{
    gGTGameMgr.shutDown();
    gGTUserServer.stop();
	gGTLoginClient.close();
}

void GateServer::memoryLog() 
{

}
void GateServer::setStop()
{
	_wait_stop = true;
}
void GateServer::checkStop()
{
	shutDown();
//	_stop = true;
}
bool GateServer::initDataFromDatabase(DBQuery* p, const void* data)
{
	if (!p)
	{ 
		return false;
	}

	net_info user_net;
	DBQuery& query = *p;
	DBQParms parms;

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
    gGTLoginClient.setNetInfo(user_net);
	gGTLoginClient.connect( user_net._ip.c_str(), user_net._port);
	Mylog::log_server( LOG_INFO, "connect login[%s] port[%u]!", user_net._ip.c_str(), user_net._port);

    parms.clear();
    query.reset();
    parms << m_service_conf.id;
    parms << _GATE_LISTEN_USER_;
    query << "SELECT * FROM `net_config` WHERE `ServerId` = %0 AND `Category` = %1";
    query.parse();

    if (!ServerFrame::getNetConfig(user_net, query.use(parms)))
    { 
        Mylog::log_server( LOG_ERROR, "Init login server net config failed !");
        return false;
    }

    if(!gGTUserServer.create( user_net._port, user_net._max_connect, user_net._thread_count))
    {
        Mylog::log_server( LOG_ERROR, "user server Init NetConfig failed!");
        return false;
    }
    gGTUserServer.setNetInfo(user_net);
    Mylog::log_server( LOG_INFO, "UserListen begin lister port[%d], MaxConnect[%d], ThreadCount[%d]!", user_net._port, user_net._max_connect, user_net._thread_count);
	
    
    parms.clear();
    query.reset();
    parms << _GAME_LISTEN_GATE_;
    query << "SELECT * FROM `net_config` WHERE `Category` = %0";
    query.parse();
    u16 nCount = 0;
    UDBResult qResult = query.use(parms);
    while (DBRow row = qResult.fetch_row()) 
    {
        user_net._id = row["ServerId"];
        user_net._port = row["Port"];
        user_net._max_connect = row["MaxConnect"];
        user_net._thread_count = row["ThreadCount"];
        user_net._ip = row["Ip"].c_str( );
        user_net._time_out = row["OutTime"];

        if (!gGTGameMgr.addClientByConfig(user_net))
        { return false;}

        nCount ++;
    }
    Mylog::log_server( LOG_INFO, "Conn Game Server count [%d]!", nCount);
    return true;
}
