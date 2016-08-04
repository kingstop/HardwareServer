#include <time.h>
#include "database.h"
#include "boost/bind/bind.hpp"


struct db_thread_task_manager
{
	db_thread_task_manager()
	{
		_works.clear();
	}
	std::list<DBTask*> _works;
	boost::mutex _mutex;
	DBTask* getTask( )
	{		
		boost::mutex::scoped_lock lock(_mutex);
		if (_works.size())
		{
			DBTask* task = _works.front();
			_works.pop_front();
			return task;
		}
		return NULL;
	}
	u32 getTaskCount()
	{
		boost::mutex::scoped_lock lock(_mutex);
		return _works.size();
	}
	void addTask(DBTask* task)
	{
		if (task)
		{
			boost::mutex::scoped_lock lock(_mutex);
			_works.push_back( task);
		}
	}
};

class db_thread
{
	friend class Database;
public:
	db_thread( Database* pkFather, db_thread_task_manager* pkdata)
	:_father( pkFather ), _stop( false ), _data( pkdata), _task(NULL), _connect(false)
	{
		_mysql = new mysqlpp::Connection;
		_mysql->set_option(new mysqlpp::MultiStatementsOption(true));
		_mysql->set_option(new mysqlpp::SetCharsetNameOption("utf8"));
		_thread = new boost::thread(boost::bind(&db_thread::run, this));
	}
	~db_thread()
	{
		delete _mysql;
		delete _thread;
	}
	bool connetDatabase();
	bool realConnect();
	void run();
	void work();
	bool isDone();
protected:
	mysqlpp::Connection* _mysql;
	boost::thread* _thread;
	bool _stop;
	bool _connect;
	Database* _father;
	db_thread_task_manager* _data;
	DBTask* _task;
private:
};

bool db_thread::realConnect()
{
	try
	{
		if (!_connect)
		{
			_mysql->disconnect();
			_mysql->set_option(new mysqlpp::MultiStatementsOption(true));
			_mysql->set_option(new mysqlpp::SetCharsetNameOption("utf8"));
			_connect = _mysql->connect( _father->_db_conf._db.c_str(), _father->_db_conf._adrs.c_str(), _father->_db_conf._user.c_str(), _father->_db_conf._pwd.c_str(), _father->_db_conf._port);
			return _connect;
		}else
		{
			return true;
		}
	}
	catch (const mysqlpp::Exception& er) 
	{
 		Mylog::log_database( LOG_ERROR, "mysql connect error: %s", er.what());
		return false;
	}
	catch(...)
	{
        Mylog::log_database(  LOG_ERROR, "mysql connect catch exception");
		return false;
	}
}
bool db_thread::isDone()
{
	boost::mutex _mutex;
	boost::mutex::scoped_lock lock(_mutex);
	return _task == NULL;
}
bool db_thread::connetDatabase()
{
	//一直调用递归，连接服务器。
	while( !_connect && !_stop)
	{
#ifdef WIN32
		Sleep(1000);
#else
		usleep(1000 * 1000);
#endif
		realConnect();

		if (_connect)
		{   
			Mylog::log_database( LOG_INFO, "connect mysql success!");
		}
	}
	return true;
}
void db_thread::run()
{
	while(!_stop && connetDatabase())
	{
		work();
	}
}

void db_thread::work()
{
	if (_task == NULL )
	{
		_task = _data->getTask();
	}
	while( _task )
	{
		if (!_mysql->connected())
		{ 
			Mylog::log_database( LOG_ERROR, "mysql connect disconnect..!");
            return ;
		}
			
		try
		{
			switch ( _task->store)
			{
			case _STORE_QUERY_:
				{
					mysqlpp::Query query= _mysql->query( _task->str.c_str());
					query.parse();
					_task->result.sr = new SDBResult( query.store( _task->arg) );
					FREESTOREPROCEDURE(query);
					query.reset();
					_task->state = DBTask::_done_;
				}break;
			case _USE_QUERY_:
				{
					mysqlpp::Query query= _mysql->query( _task->str.c_str());
					query.parse();
					_task->result.ur = new UDBResult(query.use( _task->arg));
					query.reset();
					_task->state = DBTask::_done_;
				}break;
			case _TRANCE_QUERY:
				{
						mysqlpp::Transaction trans( *_mysql );
						mysqlpp::Query query= _mysql->query( _task->str.c_str());
						_task->cb->doTranslation(&query, _task->parms);
						trans.commit();
						_task->state = DBTask::_done_;
				}break;
			case _BATCH_QUERY_:
				{
					mysqlpp::Query query= _mysql->query(/* _task->str.c_str()*/);
					_task->cb->doTranslation(&query, _task->parms);
					_task->state = DBTask::_done_;
				}break;
			default:
				{
					_task->state = DBTask::_error_;

                    Mylog::log_database( LOG_ERROR, "mysql Task type is error!");

				}break;
			}
			_father->addDone(_task);
			_task = _data->getTask();
		}
		catch (const mysqlpp::BadQuery& er) 
		{
			int errCode = er.errnum();
			Mylog::log_database( LOG_ERROR, "mysql Query error: %s, code:%d  sql:%s", er.what(), er.errnum(), _task != NULL ? _task->str.c_str() : "");
				
			if (errCode != 2013 && errCode != 2006)
			{
				_task->state = DBTask::_error_;
				_father->addDone(_task);
				_task = NULL ;
			}
			_connect = false;
			return; 
		}
		catch (const mysqlpp::BadConversion& er) 
		{
			Mylog::log_database( LOG_ERROR, "mysql Conversion error: %s retrieved data size:%d actual size:%d sql:%s", er.what(), er.retrieved, er.actual_size, _task != NULL ? _task->str.c_str() : "");
            _task->state = DBTask::_error_;
            _father->addDone(_task);
            _task = NULL ;
			return ;
		}
		catch (const mysqlpp::Exception& er) 
		{
			Mylog::log_database( LOG_ERROR, "mysql error: %s sql:%s", er.what(), _task != NULL ? _task->str.c_str() : "");
            _task->state = DBTask::_error_;
            _father->addDone(_task);
            _task = NULL ;
			return ;
		}
		catch(std::exception &er) 
		{
			Mylog::log_database( LOG_ERROR, "std::exception ERROR: %s sql:%s", er.what(),_task != NULL ? _task->str.c_str() : "");
			return;
		}
		catch(...)
		{
			Mylog::log_database( LOG_ERROR, "mysql exception sql:%s", _task != NULL ? _task->str.c_str() : "");
			return;
		}
	}
#ifdef _WIN32
	Sleep(1);
#else
	usleep(1000);
#endif
}


//////////////////////////////////////////////////////////////////////////
Database::Database() 
{

}

Database::~Database()
{
	
}
void Database::addThread(u32 ntype, u32 count)
{
	db_thread_task_manager* pkData = new db_thread_task_manager;
	_tasks[ntype] = pkData;

	if (ntype != _NORMAL_THREAD)
	{
		db_thread* _db_thread = new db_thread(this, pkData);
		_dbthread_list.push_back(_db_thread);
	}else
	{
		for (u32 i = 0; i < count; i++)
		{ 
			db_thread* _db_thread = new db_thread(this, pkData);
			_dbthread_list.push_back(_db_thread);
		}
	}	
}
void Database::addRealTask(u32 ntype, DBTask* task)
{
	std::map<u32, db_thread_task_manager*>::iterator it = _tasks.find( ntype );
	if ( it != _tasks.end())
	{
		it->second->addTask( task );
	}else
	{
		addThread(ntype);
		addRealTask(ntype, task);
	}
}
bool Database::initDatabase(const DataBaseConfig& _conf)
{
	_db_conf = _conf;
	addThread( _NORMAL_THREAD , _db_conf._thread_);
	setObjState(_run_obj_);
	return true;
}
void Database::releaseTask( DBTask* task)
{
	delete task;
}

void Database::addDone(DBTask* task)
{
	boost::mutex::scoped_lock lock(_mutex_done);
	_results.push_back( task );
}



void Database::checkStop()
{
	if (isObjState(_wait_stop_))
	{
		u32 nCount = 0;
		std::map<u32, db_thread_task_manager*>::iterator it  = _tasks.begin();
        std::map<u32, db_thread_task_manager*>::iterator itend  = _tasks.end();
		for (it; it != itend; ++it)
		{
			db_thread_task_manager* pkData = it->second;
			if (pkData)
			{ 
				nCount += pkData->getTaskCount();
				if (nCount > 0)
				{ return ;}
			}
		}

		//if (nCount > 0)
		//{
			//Mylog::log_database( LOG_PRIORITY_INFO, "DataBase:Task[%u] in witting list.", nCount);
			//return ;
		//}
	

		std::list<db_thread*>::iterator it2 = _dbthread_list.begin();
        std::list<db_thread*>::iterator it2end = _dbthread_list.end();
		for (it2; it2 != it2end; ++it2)
		{
			db_thread* pkThread = *it2;
			if (pkThread)
			{ 
				if (pkThread->isDone())
				{
					pkThread->_stop = true;
                    if (pkThread->_thread)
                    {   pkThread->_thread->join();}
				}else
				{
					return ;
				}
			}
		}

		setObjState(_stop_obj_ );
	}
}
void Database::run(u32 delay)
{
	checkStop();

	if ((isObjState(_wait_stop_) || isObjState(_run_obj_)) &&_results.size())
	{
		std::list<DBTask*> tem;
		tem.clear();
		{
			boost::mutex::scoped_lock lock(_mutex_done);
			tem = _results;
			_results.clear();
		}

		while(tem.size())
		{
			DBTask* pktask = tem.front();
			tem.pop_front();
			if (pktask)
			{
				switch ( pktask->store)
				{
				case _STORE_QUERY_:
					{
						pktask->cb->execute( pktask->result.sr, pktask->parms, pktask->state == DBTask::_done_ );
					}break;
				case _USE_QUERY_:
					{
						pktask->cb->execute( pktask->result.ur, pktask->parms , pktask->state == DBTask::_done_);
					}break;
				case _TRANCE_QUERY:
					{
						pktask->cb->execute( pktask->parms, pktask->state == DBTask::_done_);
					}break;
				case _BATCH_QUERY_:
					{
						pktask->cb->execute( pktask->parms, pktask->state == DBTask::_done_);
					}break;
				default:
					{
						//add log... error
					}
				}
				releaseTask( pktask);
			}
		}
	}
}
void Database::shutDown()
{
	//_thread_group.join_all();
	std::map<u32, db_thread_task_manager*>::iterator it = _tasks.begin();
	for (it; it != _tasks.end(); ++it)
	{
		delete it->second;
	}
	_tasks.clear();

	std::list<db_thread*>::iterator it2 =  _dbthread_list.begin();
	for (it2; it2 != _dbthread_list.end(); ++it2)
	{
		delete (*it2);
	}
	_dbthread_list.clear();
}

u32 DBDateTime2U32(const DBDateTime& dt)
{
	struct tm tmd;
	tmd.tm_sec = dt.second();
	tmd.tm_min = dt.minute();
	tmd.tm_hour = dt.hour();
	tmd.tm_mday = dt.day();
	tmd.tm_mon = dt.month() - 1;
	tmd.tm_year = dt.year() - 1900;
	tmd.tm_isdst = -1;
	return (u32)mktime(&tmd);
}
u32 DBTime2U32(const DBTime& t)
{
	return t.hour() *  3600 + t.minute() * 60 + t.second();
}