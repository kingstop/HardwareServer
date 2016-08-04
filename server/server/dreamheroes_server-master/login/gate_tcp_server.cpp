#include "stdafx.h"
#include "gate_session.h"

GateTcpServer::GateTcpServer(void) : tcp_server( 0 )
{

}

GateTcpServer::~GateTcpServer(void)
{
}

tcp_session* GateTcpServer::create_session()
{
	return new GateSession;
}

void GateTcpServer::run()
{
	tcp_server::run();
}
