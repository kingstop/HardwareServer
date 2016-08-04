#ifndef __client_manager_h__
#define __client_manager_h__

enum enClientType
{
	offline_client,
	online_client
};



class Client;
typedef std::map<std::string, Client*> MAP_CLIENTS;
class ClientTest : public ServerFrame , public EventableObject
{
public:
    ClientTest();
    virtual bool init(int accountBegin, int accountEnd);
    virtual bool onKey() ;
    virtual void runOnce(u32 nDiff) ;
    virtual void shutDown() ;
    virtual void memoryLog() ;
    virtual void setStop(); 
    virtual void checkStop();

    void addRobot();
    void reConnetClient();
    void addToReconn(Client* p);
    void removeFromReconn(Client* p);
	void addOnlineClient(Client* p);
	void addOfflineClient(Client* p);
	bool isStop();

	void ClientListModify(const char* sztemp, enClientType en, bool add);

protected:
    u32 m_maxCount;
    u32 m_curCount;
    u32 m_onceCount;
    u32 m_Online;
    u32 m_onceTime;
    u32 m_TransCount;
    u32 m_accountBegin;
	bool _stop;
	volatile bool _client_init;
    EventableObjectHolder m_EventHold;
    std::vector<Client*> m_Clients;
    std::list<Client*>m_WaitReconnet;
    std::set<Client*>m_Reconns;
	MAP_CLIENTS _offline_clients;
	MAP_CLIENTS _online_clients;
    net_info m_info;
private:
};

#endif