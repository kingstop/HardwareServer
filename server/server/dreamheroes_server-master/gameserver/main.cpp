#include "stdafx.h"

#ifdef WIN32

#include "windump.h"

#endif // WIN32



#include "gate_session.h"

#include "session.h"



int safe_return(int n)

{

#ifdef _WIN32

	std::string str;

	printf("server shutdown! \n");



	printf("please enter <quit> to exit \n" );

	std::cin >> str;

	while (str != "quit")

	{

		std::cin >> str;

		Sleep(100);

	}



#endif   



	return n;



}

FuGameFather* gFuGameFather = NULL ;

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

	GameDBClient::initPBModule();

	GateSession::initPBModule();

	Session::registerPBCall();



	service_config conf;

	if(!ServerFrame::loadServiceConfig(conf, SERVER_CONFIG))

	{

		printf("loadServiceConfig file <%s> failed \n", SERVER_CONFIG );

		return false;

	}

	net_global::init_net_service( conf.thread_count, conf.proc_interval, NULL, conf.speed_, conf.msg_pool_size);

	gFuGameFather = new FuGameFather;



	gGameServer.setServiceConf(conf);

	if (!gGameServer.init())

	{

		printf("game server init  failed \n" );

		return false;

	}



	return true;

}

void run()

{

	gGameServer.run();



}



void shutdown()

{

	gGameServer.shutDown();

	delete gFuGameFather;

	gFuGameFather = NULL;

	net_global::free_net_service();

}



int main()

{

#ifdef WIN32

	MiniDumper dump("game",Version::toString().c_str(),"game server error");

#endif

	if (init())

	{

		run();

		shutdown();

	}

	else

	{

		printf("init server failed \n" );

	}



	return safe_return(1);



}

