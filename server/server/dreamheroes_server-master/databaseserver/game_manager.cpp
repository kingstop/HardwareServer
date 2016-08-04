#include "stdafx.h"
#include "game_session.h"

GameManager::GameManager()
{
	memset( m_Games, 0 , sizeof(GameSession*) * MAX_GATE_ID);
}

GameManager::~GameManager()
{
}

GameSession* GameManager::getGameClient(u16 gs)
{
	if (gs > INVALID_GATE_ID && gs < MAX_GATE_ID )
	{
		return m_Games[gs];
	}
	return NULL;
}

bool GameManager::addGame(u16 nId, GameSession* p)
{
	if (!p)
	{
		Mylog::log_server( LOG_ERROR, "Add Game[%d] error!  Game Pointer is null", nId);
		return false;
	}

	if (nId > INVALID_GATE_ID && nId < MAX_GATE_ID )
	{
		m_Games[nId] = p;
		Mylog::log_server( LOG_INFO, "Add Game[%d] Active", nId);
		return true;
	}

	Mylog::log_server( LOG_ERROR, "Add Game[%d] error!", nId);
	return false;
}

void GameManager::removeGame(GameSession* p)
{
	if (p)
	{
		u16 nId = p->getGameId();
		if (nId > INVALID_GATE_ID && nId < MAX_GATE_ID )
		{
			if (m_Games[nId] == p)
			{
				m_Games[nId] = NULL;
				Mylog::log_server( LOG_ERROR, "Remove Active Game[%d] ", nId);
			}
		}
	}
}

void GameManager::sendMsgToAll(google::protobuf::Message* msg, pb_flag_type flag)
{
	for (u16 i = 0 ; i < MAX_GATE_ID; ++i)
	{
		if (m_Games[i])
		{
			m_Games[i]->sendPBMessage(msg, flag);
		}
	}
}

void GameManager::sendMessage(google::protobuf::Message* msg, pb_flag_type flag, u16 gs)
{
	if (gs > INVALID_GATE_ID && gs < MAX_GATE_ID )
	{
		if (m_Games[gs] )
		{
			m_Games[gs]->sendPBMessage(msg, flag);
			return ;
		}
	}
}