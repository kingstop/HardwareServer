#include "stdafx.h"
#include "user_session.h"
#include "gate_session.h"
#ifdef WIN32
#include "windump.h"
#endif // WIN32

FuLoginFather* gFuLoginServer = NULL ;
bool init()
{
	message::protobuf_AssignDesc_common_2eproto();
	message::protobuf_AssignDesc_login_2eproto();
	message::protobuf_AssignDesc_msgs2s_2eproto();
	message::protobuf_AssignDesc_msgs2c_2eproto();


	message::protobuf_AssignDesc_msg_5fgate_5fgame_2eproto();

    Mylog::log_init(LOG4CXX_LOG_CONFIG);
    UserLoginSession::initPBModule();
    GateSession::initPBModule();
    service_config conf;
    if(!ServerFrame::loadServiceConfig(conf, SERVER_CONFIG))
    {   
		return false;
	}
    net_global::init_net_service( conf.thread_count, conf.proc_interval, NULL, conf.speed_, conf.msg_pool_size);

    gFuLoginServer = new FuLoginFather;
    if (!gLoginServer.init())
    {    return false;}

    return true;
}
void run()
{
    gLoginServer.run();
}

void shutdown()
{
    gLoginServer.shutDown();
    delete gFuLoginServer;
    gFuLoginServer = NULL;
    net_global::free_net_service();
}

int main()
{
#ifdef WIN32
    MiniDumper dump("login",Version::toString().c_str(),"login server error");
#endif
    if (init())
    {
        run();
        shutdown();
    }
    return 1 ;
}