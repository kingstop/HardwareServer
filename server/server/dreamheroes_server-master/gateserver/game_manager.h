#ifndef __game_manager_h__
#define __game_manager_h__
class GateGameClient;
class GameManager
{
public:
    GameManager();
    ~GameManager();
    bool addClientByConfig(const net_info& conf);
    void update(u32 diff);
    void shutDown();
    void addActiveGameClient(GateGameClient* pkConn, u32 nId);
    void removeActiveGameClient( GateGameClient* pkConn);

    bool isGamesStop();
    GateGameClient* getIdleGameClient();
    GateGameClient* getGameClient(u16 gs);
protected:
private:
    GateGameClient* m_activeGames[MAX_GAME_ID];
    std::set<GateGameClient*> m_Games;
};
#endif