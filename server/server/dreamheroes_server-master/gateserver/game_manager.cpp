#include "stdafx.h"
#include "game_client.h"

GameManager::GameManager()
{
    for (u16 i = 0; i < MAX_GAME_ID; ++i)
    {
        m_activeGames[i]= NULL;
    }
}
GameManager::~GameManager()
{
    for (u16 i = 0; i < MAX_GAME_ID; ++i)
    {   m_activeGames[i]= NULL;}

    std::set<GateGameClient*>::iterator it = m_Games.begin();
    std::set<GateGameClient*>::iterator itend = m_Games.end();

    for (it; it!= itend; ++it)
    {
        (*it)->close();
        delete (*it);
    }

    m_Games.clear();
}
bool GameManager::addClientByConfig(const net_info& conf)
{
    GateGameClient* p = new GateGameClient;
    p->setNetInfo(conf);
    p->connect(conf._ip.c_str(), conf._port);
    m_Games.insert(p);
    return true;
}

void GameManager::update(u32 diff)
{
    std::set<GateGameClient*>::iterator it = m_Games.begin();
    std::set<GateGameClient*>::iterator itend = m_Games.end();

    for (it; it!= itend; ++it)
    {
        (*it)->run_no_wait();
    }
}
void GameManager::shutDown()
{
    for (u16 i = 0; i < MAX_GAME_ID; ++i)
    {   m_activeGames[i]= NULL;}

    std::set<GateGameClient*>::iterator it = m_Games.begin();
    std::set<GateGameClient*>::iterator itend = m_Games.end();
    for (it; it!= itend; ++it)
    {
        (*it)->close();
        delete (*it);
    }
     m_Games.clear();
}

void GameManager::addActiveGameClient(GateGameClient* pkConn, u32 nId)
{
    if (!pkConn)
    {
        Mylog::log_server( LOG_ERROR, "Add GameServer[%d] error!  GameServer Pointer is null", nId);
        return ;
    }
    if (nId > 0 && nId < MAX_GAME_ID)
    {
        m_activeGames[ nId ] = pkConn ;
        Mylog::log_server( LOG_INFO, "Add GameServer[%d] Active", nId);
        return ;
    }

    Mylog::log_server( LOG_ERROR, "Add GameServer[%d] error!", nId);
}
void GameManager::removeActiveGameClient( GateGameClient* pkConn)
{
    if (!pkConn)
    {
        Mylog::log_server( LOG_ERROR, "Remove GameServer error!  GameServer Pointer is null");
        return ;
    }
    u32 nId = pkConn->getGameId();
    if (nId > 0 && nId < MAX_GAME_ID)
    {
        if (m_activeGames[ nId] == pkConn)
        {
            gGTUserMgr.removeUsers( pkConn->getGameId());
            m_activeGames[ nId ] = NULL ;
            Mylog::log_server( LOG_INFO, "Remove Active GameServer[%d] ", nId);
        }else
        {
            Mylog::log_server( LOG_ERROR, "Remove Active GameServer[%d] error! GameServer is not found", nId);
        }
    }
    Mylog::log_server( LOG_ERROR, "Remove GameServer[%d] error!", nId);
}
bool  GameManager::isGamesStop()
{
    for (u16 i = 0; i < MAX_GAME_ID; ++i)
    {
        if (NULL != m_activeGames[i])
        {
            return false;
        }
    }
    return true;
}
GateGameClient* GameManager::getGameClient(u16 gs)
{
    if (gs > INVALID_GAME_ID && gs < MAX_GAME_ID)
    {
        return m_activeGames[gs];
    }else
    {
        return NULL ;
    }
}
GateGameClient* GameManager::getIdleGameClient()
{
    GateGameClient* pkConn = NULL;
    for (u16 i = 0 ; i < MAX_GAME_ID; i++)
    {
        if (m_activeGames[ i ] != NULL )
        {
            if (pkConn == NULL)
            {
                pkConn = m_activeGames[i];

                if (pkConn->getOnlines() < gGateServer.getGSIdleMax())
                {   return pkConn;}

            }else
            {
                if (pkConn->getOnlines() > m_activeGames[i]->getOnlines())
                {
                    pkConn = m_activeGames[ i ];

                    if (pkConn->getOnlines() < gGateServer.getGSIdleMax())
                    {   return pkConn;}
                }
            }
        }
    }

    return pkConn;
}

