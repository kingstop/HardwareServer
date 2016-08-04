#ifndef __db_quest_h__
#define __db_quest_h__
#include <event_table_object.h>

class DBQuestManager : public EventableObject
{
public:
	DBQuestManager();
	~DBQuestManager();
public:
	void queryHeroInfo(account_type a, tran_id_type t, u16 gs);
	void saveHeroMap(message::MsgSaveHeroDataGS2DB* msg);
	void saveSqlData(const char* sql);
	void saveToClose(u16 gsid);
	void verifyToyCDKey(tran_id_type t, u16 gs, account_type acc, const char* cdkey);
private:
	void onSaveToClose();
protected:
	void dbDoQueryHeroInfo(const SDBResult* r, const void* d, bool s);
	void dbDoQueryHeroEquips(const SDBResult* r, const void* d, bool s);
	void dbDoQuerryHeroToys(const SDBResult* r, const void* d, bool s);
	void dbDoQuerryToyVerify(const SDBResult* r, const void* d, bool s);
protected:
	void dbCallNothing(const SDBResult*, const void*, bool) { ; }
public:
	bool _receive_cose_msg;
};
#endif

