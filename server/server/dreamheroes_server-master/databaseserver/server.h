#ifndef __server_h__
#define __server_h__
#include "common_type.h"
#include "server_frame.h"
#include "common_template.h"
#include "event_table_object.h"



class DBServer : public ServerFrame
{
public:
	DBServer();
	virtual bool init() ;
	virtual bool onKey() ;
	virtual void runOnce(u32 nDiff) ;
	virtual void shutDown() ;
	virtual void memoryLog() ;
	virtual void setStop(); 
	virtual void checkStop();
	virtual void signalStop();
	bool initDataFromDatabase(DBQuery* query, const void* data);
	bool initDataFromCharDB(DBQuery* p, const void* data);
	bool initDataFromWorldDB(DBQuery* p, const void* data);
	
private:
	
protected:
	DataBaseConfig m_world_db_conf;
	EventableObjectHolder m_EventHold;
private:

};

#endif