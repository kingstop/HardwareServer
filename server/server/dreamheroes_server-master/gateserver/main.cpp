#include "stdafx.h"
#include "login_client.h"
#include "game_client.h"
#include "user_session.h"

//#include "message/map.pb.h"
#ifdef WIN32
#include "windump.h"
#endif // WIN32

FuGateFather* gFuGateServer = NULL ;
bool init()
{

	message::protobuf_AssignDesc_common_2eproto();
	message::protobuf_AssignDesc_login_2eproto();
	message::protobuf_AssignDesc_msgs2s_2eproto();
	message::protobuf_AssignDesc_msgs2c_2eproto();
	message::protobuf_AssignDesc_msg_5fgame_5fdb_2eproto();
	message::protobuf_AssignDesc_msg_5fgate_5fgame_2eproto();
	message::protobuf_AssignDesc_msg_5fgame_5fdb_2eproto();
	message::protobuf_AssignDesc_dream_5fhero_2eproto();
    Mylog::log_init(LOG4CXX_LOG_CONFIG);
    service_config conf;
    if(!ServerFrame::loadServiceConfig(conf, SERVER_CONFIG))
    {   
		return false;
	}
    net_global::init_net_service( conf.thread_count, conf.proc_interval, NULL, conf.speed_, conf.msg_pool_size);
    GateLoginClient::initPBModule();
    GateGameClient::initPBModule();
    UserSession::initPBModule();
    gFuGateServer = new FuGateFather;
    gGateServer.setServiceConf(conf);
    if (!gGateServer.init())
    { 
		return false;
	}

	

    return true;
}
void run()
{
    gGateServer.run();
    
}

void shutdown()
{
    gGateServer.shutDown();
    delete gFuGateServer;
    gFuGateServer = NULL;
    net_global::free_net_service();
}

int main()
{
#ifdef WIN32
    MiniDumper dump("gate",Version::toString().c_str(),"gate server error");
#endif
    if (init())
    {
        run();
        shutdown();
    }
    return 1 ;
}