#include <stdio.h>
#include <stdarg.h>
#include "my_log.h"
//#ifndef WIN32
#include "MyLog.h"
//#else
//#include "log4cxx/logger.h"
//#include "log4cxx/propertyconfigurator.h"
//#endif


#include "boost/thread/mutex.hpp"


enum
{
    _server_log_ ,
    _exception_log_,
    _memory_log_,
    _database_log_,
    _player_log_,
    _gm_log_,
    _bill_log_,
    _mail_log_,
    _chat_log_,
    _monster_log_,
    _robot_log_,
    _room_log_,
    _vp_log_,
    _store_log_,

    _max_log_ptr_,
}; 
const char* log_define[_max_log_ptr_] =
{
    "Server.log",
    "Exception.log",
    "Memory.log",
    "DataBase.log",
    "Player.log",
    "GM.log",
    "Billing.log",
    "mail.log",
    "chat.log",
    "Monster.log",
    "robot.log",
    "Room.log",
    "VP.log",
    "store.log",
}; 

boost::mutex s_log_buff_mutex;
char s_log_buff[4096] ;
bool Mylog::log_init(const char* file)
{
//#ifndef WIN32
	ShareSpace::MyLog::Log::log_init(file);
//#else
//	log4cxx::PropertyConfigurator::configure(file);
//#endif
    
    return true;
}

void log_print(unsigned int index, int priority,  const std::string& str)
{
    if (index >= _server_log_ && index < _max_log_ptr_)
    {
//#ifndef WIN32
		try
		{
			switch(priority)
			{
			case LOG_CRITICAL:
				ShareSpace::MyLog::Log::log_fatal(str);
				break;
			case LOG_ERROR:
				ShareSpace::MyLog::Log::log_error(str);
				break;
			case LOG_WARNING:
				ShareSpace::MyLog::Log::log_warning(str);
				break;
			case LOG_INFO:
				ShareSpace::MyLog::Log::log_info(str);				
				break;
			case LOG_DEBUG:
				ShareSpace::MyLog::Log::log_debug(str);
				break;
			default:
				ShareSpace::MyLog::Log::log_info(str);
				break;
			}
		}
		catch (...)
		{
			return ;
		}
//#else
//		log4cxx::LoggerPtr p = log4cxx::Logger::getLogger(log_define[index]);
//		if (p)
//		{
//			try 
//			{
//				switch(priority)
//				{
//				case LOG_CRITICAL:
//					p->fatal(str);
//					break;
//				case LOG_ERROR:
//					p->error(str);
//					break;
//				case LOG_WARNING:
//					p->warn(str);
//					break;
//				case LOG_INFO:
//					p->info(str);
//					break;
//				case LOG_DEBUG:
//					p->debug(str);
//					break;
//				default:
//					p->info(str);
//					break;
//				};
//			}
//			catch (...)
//			{
//				return ;
//			}
//		}
//#endif
    }
}
void Mylog::log(int priority, unsigned int index, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(index, priority, s_log_buff);
}
void Mylog::log_error(unsigned int index, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(index, LOG_ERROR, s_log_buff);
}
void Mylog::log_debug(unsigned int index, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(index, LOG_DEBUG, s_log_buff);
}
void Mylog::log_warning(unsigned int index, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(index, LOG_WARNING, s_log_buff);
}
void Mylog::log_info(unsigned int index, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(index, LOG_INFO, s_log_buff);
}
void Mylog::log_critical(unsigned int index, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(index, LOG_CRITICAL, s_log_buff);
}

void Mylog::log_server(int priority, const char* format, ... )
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_server_log_, priority, s_log_buff);
}
void Mylog::log_exception(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_exception_log_, priority, s_log_buff);
}
void Mylog::log_memory(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_memory_log_, priority, s_log_buff);
}
void Mylog::log_database(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_database_log_, priority, s_log_buff);
}

void Mylog::log_gm(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_gm_log_, priority, s_log_buff);
}
void Mylog::log_bill(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_bill_log_, priority, s_log_buff);
}
void Mylog::log_mail(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_mail_log_, priority, s_log_buff);
}
void Mylog::log_chat(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_chat_log_, priority, s_log_buff);
}

void Mylog::log_player(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_player_log_, priority, s_log_buff);
}
void Mylog::log_monster(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args ); 
    log_print(_monster_log_, priority, s_log_buff);
}
void Mylog::log_room(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_room_log_, priority, s_log_buff);
}
void Mylog::log_store(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_store_log_, priority, s_log_buff);
}
void Mylog::log_robot(int priority, const char* format, ...)
{
    boost::mutex::scoped_lock lock(s_log_buff_mutex);
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_robot_log_, priority, s_log_buff);
}
void Mylog::log_vplayer(int priority, const char* format, ...)
{
    va_list args;
    va_start( args, format );
    vsprintf( s_log_buff,format, args );
    va_end( args );
    log_print(_vp_log_, priority, s_log_buff);
}
void Mylog::log_release()
{

}
