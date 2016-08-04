#include "stdafx.h"
#include "user_session.h"

UserTcpServer::UserTcpServer(void) : tcp_server( 0 )
{

}

UserTcpServer::~UserTcpServer(void)
{
}

tcp_session* UserTcpServer::create_session()
{
	return new UserLoginSession;
}

void UserTcpServer::run()
{
	tcp_server::run();
}

