#ifndef __server_h__
#define __server_h__

class LoginServer : public ServerFrame
{
public:
    LoginServer();
    ~LoginServer();
    virtual bool init() ;
    virtual bool onKey() ;
    virtual void runOnce(u32 nDiff) ;
    virtual void shutDown() ;
    virtual void memoryLog() ;
    virtual void setStop(); 
    virtual void checkStop();

    bool initDataFromDatabase(DBQuery* query, const void* data);
private:
    EventableObjectHolder m_EventHold;
protected:
private:
};
#endif