#ifndef __server_h__
#define __server_h__
class GateServer : public ServerFrame
{
public:
    GateServer();
	virtual bool init() ;
	virtual bool onKey() ;
	virtual void runOnce(u32 nDiff) ;
	virtual void shutDown() ;
	virtual void memoryLog() ;
	virtual void setStop(); 
	virtual void checkStop();

    u32 getGSIdleMax(){return m_service_conf.limit_player;}
    void setServiceConf( const service_config& c){m_service_conf = c;}
    const service_config& getServiceConf() const{return m_service_conf;}

	bool initDataFromDatabase(DBQuery* query, const void* data);
private:
    EventableObjectHolder m_EventHold;
protected:
    service_config m_service_conf;
private:
};
#endif