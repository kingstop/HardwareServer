#ifndef __database_h__
#define __database_h__
#include "common_header.h"
#include "run_object.h"
#include <boost/asio.hpp>
#include "mysql++.h"
#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"
#include "my_log.h"
class  db_thread;
struct db_thread_task_manager;

typedef mysqlpp::StoreQueryResult SDBResult;
typedef mysqlpp::UseQueryResult   UDBResult;
typedef mysqlpp::SQLQueryParms    DBQParms;
typedef mysqlpp::Query			  DBQuery;
typedef mysqlpp::Row			  DBRow;
typedef mysqlpp::DateTime         DBDateTime;
typedef mysqlpp::Time			  DBTime;

#define MYSQLPPBIGINT(x) mysqlpp::sql_bigint_unsigned((x))
#define MYSQLPPDATATIME(x) mysqlpp::sql_datetime((x))     

extern u32 DBDateTime2U32(const DBDateTime& dt);
extern u32 DBTime2U32(const DBTime& t);

#define FREESTOREPROCEDURE(mysqlQuery) \
	while(mysqlQuery.more_results()) \
	{ mysqlQuery.store_next(); } \

enum
{
	_STORE_QUERY_ = 0,
	_USE_QUERY_		 ,
	_TRANCE_QUERY	 ,
	_BATCH_QUERY_,

};
enum
{
    _NORMAL_THREAD = 0,
};
struct DataBaseConfig
{
    DataBaseConfig()
    {
		_id = 0;
        _db = "db";
        _adrs = "127.0.0.1";
        _user = "";
        _pwd = "";
        _port = 0;
        _thread_ = 0;
    }
	u8 _id;
    std::string _db;
    std::string _adrs;
    std::string _user;
    std::string _pwd;
    u32 _port;
    u32 _thread_;
};

union DBResult
{
	const SDBResult* sr;
	const UDBResult* ur;
};

struct DBCallback
{
	virtual void execute(const SDBResult* result, const void* parms, bool sucess) {;}
	virtual void execute(const UDBResult* result, const void* parms, bool sucess) {;}
	virtual void execute(const void* parms, bool sucess){;}
	virtual void doTranslation(DBQuery* query, const void* parms){;}
};

template <class Class>
struct DBCallbackS : public DBCallback
{
	typedef void (Class::*method)(const SDBResult* , const void*, bool )  ;

	DBCallbackS(Class *obj, method f )
	{
		self = obj;
		fun = f;
	}

	virtual void execute(const SDBResult* result, const void* parms, bool sucess)
	{
		if (self && fun)
		{ (self->*fun)(result, parms, sucess) ;}
	}

	Class * self;
	method fun;
};
template <class Class>
struct DBCallbackU : public DBCallback
{
	typedef void (Class::*method)(const UDBResult* , const void*, bool)  ;

	DBCallbackU(Class *obj, method f )
	{
		self = obj;
		fun = f;
	}

	virtual void execute(const UDBResult* result, const void* parms, bool sucess)
	{
		if (self && fun)
		{ (self->*fun)(result, parms, sucess) ;}
	}

	Class * self;
	method fun;
};

template <class Class>
struct DBTransCall : public DBCallback
{
	typedef void (Class::*method)(const void*, bool )  ;
	typedef void (Class::*transfun)(DBQuery*, const void*); 

	DBTransCall(Class *obj, method f, transfun tf )
	{
		self = obj;
		fun = f;
		tfun = tf;
	}

	virtual void execute(const void* parms, bool sucess)
	{
		if (self && fun)
		{ (self->*fun)(parms, sucess) ;}
	}

	virtual void doTranslation(DBQuery* query, const void* parms)
	{
		if (self && tfun)
		{ (self->*tfun)(query, parms);}
	}
	Class * self;
	method fun;
	transfun tfun;
};

template <class Class>
struct DBBatchCall : public DBCallback
{
	typedef void (Class::*method)(const void*, bool )  ;
	typedef void (Class::*transfun)(DBQuery*, const void*); 

	DBBatchCall(Class *obj, method f, transfun tf )
	{
		self = obj;
		fun = f;
		tfun = tf;
	}

	virtual void execute(const void* parms, bool sucess)
	{
		if (self && fun)
		{ (self->*fun)(parms, sucess) ;}
	}

	virtual void doTranslation(DBQuery* query, const void* parms)
	{
		if (self && tfun)
		{ (self->*tfun)(query, parms);}
	}
	Class * self;
	method fun;
	transfun tfun;
};

struct DBTask
{
	enum
	{
		_doing_,
		_done_,
		_error_,
	};
	DBTask(DBCallback* callback, const char* st, u32 f, const DBQParms* q, const void* p)
		:cb(callback), store(f), str(st),arg(*q), parms(p),state(_doing_)
	{
		switch (store)
		{
		case _STORE_QUERY_:
			{
				result.sr = NULL;
			}break;
		case _USE_QUERY_:
			{
				result.ur = NULL;
			}break;
		case _TRANCE_QUERY:
			{

			}break;
		case  _BATCH_QUERY_:
			{

			}break;
		default:
			{

			}break;
		}
	}

	DBTask( DBCallback* callback, u32 f, const void* p, const char* st)
		:cb(callback),str(st), store(f), parms( p),state(_doing_)
	{
		result.sr = NULL;
	}

	~DBTask()
	{
		switch (store)
		{
		case _STORE_QUERY_:
			{
				if (result.sr)
				{
					delete result.sr;
					result.sr = NULL;
				}
			}break;
		case _USE_QUERY_:
			{
				if (result.ur)
				{
					delete result.ur;
				}
				result.ur = NULL;
			}break;
		case _TRANCE_QUERY:
			{

			}break;
		case  _BATCH_QUERY_:
			{

			}break;
		default:
			{

			}break;
		}

		if (cb)
		{ delete cb;}
	}
	DBCallback* cb;				//�ص�����
	std::string str;			//ִ�����
	DBResult result;			//ִ�н��
	DBQParms arg;				//���Ӳ���
	u32 store;					//�������
	const void* parms;			//�ص�������������	
	u8 state;
};

//���ݿ����ȫ�����ǲ��ûص������ķ�ʽ���в���
//�����ݿ����ִ���ڼ�,�벻Ҫ�ͷŴ������Դ
//�����ݿ������ص������������,����ִ���쳣��ʱ��,�ǵ��ͷ����Ӧ����Դ
//�ص������е�bool�ֶ�,��ʶִ���Ƿ�ɹ�,������ɹ�,���ǳ������쳣���
class Database : public RunObject
{
	friend class db_thread;
public:
	Database();
	virtual ~Database();
public:
	bool initDatabase(const DataBaseConfig& _conf);
	void shutDown();
	void run(u32 delay);
	virtual void checkStop();
	
	
	static void getString(char* in,  const mysqlpp::String& str, u32 len)
	{
		size_t l = str.length();
		memcpy(in, str.data(), len > l ? l : len );
		in[l] = 0;
	}

	void releaseTask( DBTask* task);
	void addDone(DBTask* task);
	//ִ����ͨ����
	//obj		ִ�в����İ󶨶���
	//method	�ص����� StoreQueryResult(�����������) SDBResult* �ǲ�ѯ��� �� const void*ǰ�洫�ݽ����Ļص��õĲ��� bool ��ʶ�Ƿ�ִ�гɹ�
	//str		���
	//ntype		�߳�����
	//...		�����������
	//������õ��Ǵ洢���̣�����ֻ�᷵�ص�һ����¼�������ݣ������Ҫ�����������ʹ��AddBatchTask
	template<class Class>
	void addSQueryTask(Class* obj, void(Class::*method)(const SDBResult* , const void* , bool), const char* str,  const DBQParms* parms = NULL, const void* data = NULL, u32 ntype = _NORMAL_THREAD)
	{
		if(!isObjState(_run_obj_))
		{ return ;}

		if (NULL == parms)
		{
			DBQParms tem ;
			parms = &tem;
		}
		DBTask* pkTask = new DBTask(new DBCallbackS<Class>( obj, method), str, _STORE_QUERY_, parms, data);
		addRealTask( ntype, pkTask );
	}

	//ִ����ͨ����
	//obj		ִ�в����İ󶨶���
	//method	�ص����� UseQueryResult(1��1�н������) UDBResult* �ǲ�ѯ��� �� const void*ǰ�洫�ݽ����Ļص��õĲ��� bool ��ʶ�Ƿ�ִ�гɹ�
	//str		���
	//ntype		�߳�����
	//...		�����������

	template<class Class>
	void addUQueryTask(Class* obj, void(Class::*method)(const UDBResult* , const void*, bool ), const char* str,  const DBQParms* parms = NULL, const void* data = NULL, u32 ntype = _NORMAL_THREAD)
	{
		if(!isObjState(_run_obj_))
		{ return ;}

		if (NULL == parms)
		{
			DBQParms tem ;
			parms = &tem;
		}

		DBTask* pkTask = new DBTask(new DBCallbackU<Class>( obj, method), str, _USE_QUERY_, parms, data);
		addRealTask( ntype, pkTask );
	}
	//ִ������
	//obj		ִ�в����İ󶨶���
	//tranfun	��ִ������ľ������ݿ�Ĳ�����Ϊ�����̵߳��ã�ע�ⰲȫ��
	//method	������ִ�гɹ���Ļص�����  const void*ǰ�洫�ݽ����Ļص��õĲ��� bool ��ʶ�Ƿ�ִ�гɹ�
	//parms		ִ��������Ҫ�����Ĳ���
	//ntype		�߳�����.
	template<class Class>
	void addTranceTask(Class* obj, void(Class::*tranfun)(DBQuery*, const void* ), void(Class::*method)(const void* , bool), void* data,  u32 ntype = _NORMAL_THREAD)
	{
		if(!isObjState(_run_obj_))
		{ return ;}

		DBTask* pkTask = new DBTask(new DBTransCall<Class>(obj, method, tranfun), _TRANCE_QUERY, data);
		addRealTask(ntype, pkTask);
	}

	//ִ��һ������
	//batchfun
	template<class Class>
	void addBatchTask(Class* obj, void(Class::*batchfun)(DBQuery*, const void* ), void(Class::*method)(const void* , bool), void* data,  const char* str, u32 ntype = _NORMAL_THREAD)
	{
		if(!isObjState(_run_obj_))
		{ return ;}

		DBTask* pkTask = new DBTask(new DBBatchCall<Class>(obj, method, batchfun), _BATCH_QUERY_, data, str);
		addRealTask(ntype, pkTask);
	}
	//ͬ������
	template<class Class>
	static bool addBlockTask(const DataBaseConfig& conf, Class* obj, bool(Class::*method)(DBQuery*, const void* ), const void* data = NULL);

protected:
	void addThread(u32 ntype, u32 count = 1);
	void addRealTask(u32 ntype, DBTask* task);
protected:
	DataBaseConfig _db_conf;
	std::list<DBTask*> _results;
	std::map<u32, db_thread_task_manager*> _tasks;
	boost::mutex _mutex_done;
	std::list<db_thread*> _dbthread_list;

};
template<class Class>
bool Database::addBlockTask(const DataBaseConfig& conf, Class* obj, bool(Class::*method)(DBQuery*, const void* ), const void* data)
{
	try
	{
		mysqlpp::Connection myconn;
		mysqlpp::SetCharsetNameOption* CharsetNameOption = new mysqlpp::SetCharsetNameOption("utf8");
		myconn.set_option(CharsetNameOption);
		mysqlpp::MultiStatementsOption* MultiStatements = new mysqlpp::MultiStatementsOption(true);
		myconn.set_option(MultiStatements);
		if (myconn.connect( conf._db.c_str(), conf._adrs.c_str(), conf._user.c_str(), conf._pwd.c_str(), conf._port))
		{
			DBQuery query= myconn.query();
			if (obj && method)
			{ 
				return (obj->*method)(&query, data) ;
			}
		}

	}catch (const mysqlpp::BadQuery& er) 
	{
		Mylog::log_database(LOG_ERROR, "mysql Query error: %s, code:%d", er.what(), er.errnum());
		return false;
	}
	catch (const mysqlpp::BadConversion& er) 
	{
		Mylog::log_database(LOG_ERROR, "mysql Conversion error: %s \tretrieved data size:%d actual size:%d", er.what(), er.retrieved, er.actual_size);
		return false;
	}
	catch (const mysqlpp::Exception& er) 
	{
		Mylog::log_database(LOG_ERROR, "mysql error: %s", er.what());
		return false;
	}
	catch(std::exception &er) 
	{
		Mylog::log_database(LOG_ERROR,"std::exception ERROR: %s", er.what());
		return false;
	}
	Mylog::log_database(LOG_ERROR, "connect mysql failed!");
	return false;
}

#endif