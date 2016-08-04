#include "stdafx.h"
#include "game_session.h"
GameTCPServer::GameTCPServer(void) : tcp_server( 0 )
{
}

GameTCPServer::~GameTCPServer(void)
{
}

tcp_session* GameTCPServer::create_session()
{
	return new GameSession;
}

void GameTCPServer::run()
{
	tcp_server::run();
}