#ifndef __game_manager_h__
#define __game_manager_h__
class GameSession;
class GameManager
{
public:
	GameManager();
	~GameManager();
	bool addGame(u16 nId, GameSession* p);
	void removeGame(GameSession* p);
	void sendMessage(google::protobuf::Message* msg, pb_flag_type flag, u16 gs);
	void sendMsgToAll(google::protobuf::Message* msg, pb_flag_type flag);
	//void offlineUser(tran_id_type tranid);
	//void removeAllUsers();
	GameSession* getGameClient(u16 gs);
protected:
	//void removeUserByGate(u16 nGateId);
protected:
private:
	GameSession* m_Games[MAX_GATE_ID];
};
#endif