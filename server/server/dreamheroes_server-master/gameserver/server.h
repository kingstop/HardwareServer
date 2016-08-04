#ifndef __server_h__
#define __server_h__
class GameServer : public ServerFrame
{
public:
	GameServer();
	virtual bool init();
	virtual bool onKey();
	virtual void runOnce(u32 nDiff);
	virtual void shutDown();
	virtual void memoryLog();
	virtual void setStop(); 
	virtual void checkStop();
	void setServiceConf( const service_config& c){m_service_conf = c;}
	const service_config& getServiceConf() const{return m_service_conf;}
	u8 getServerID(){return m_service_conf.id;}
	bool initDataFromDatabase(DBQuery* query, const void* data);
	bool initDataFromWorldDB(DBQuery* query, const void* data);
	bool initDataFromCharacterDB(DBQuery* p, const void* data);
private:
	service_config m_service_conf;
	DataBaseConfig m_world_db_conf;
	DataBaseConfig m_character_conf;
	EventableObjectHolder m_EventHold;
protected:
private:
};
#endif