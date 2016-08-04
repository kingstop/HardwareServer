#ifndef __my_log_h__
#define __my_log_h__

enum 
{
    LOG_CRITICAL = 0,
    LOG_ERROR    = 1,
    LOG_WARNING  = 2,
    LOG_INFO     = 3,
    LOG_DEBUG    = 4,
};

struct Mylog 
{
    static bool log_init(const char* file);
    static void log_release();

    static void log(int priority, unsigned int index, const char* format, ...);

    static void log_error(unsigned int index, const char* format, ...);
    static void log_debug(unsigned int index, const char* format, ...);
    static void log_warning(unsigned int index, const char* format, ...);
    static void log_info(unsigned int index, const char* format, ...);
    static void log_critical(unsigned int index, const char* format, ...);

    static void log_server(int priority, const char* format, ... );
    static void log_exception(int priority, const char* format, ...);
    static void log_memory(int priority, const char* format, ...);
    static void log_database(int priority, const char* format, ...);
    static void log_gm(int priority, const char* format, ...);
    static void log_bill(int priority, const char* format, ...);
    static void log_mail(int priority, const char* format, ...);
    static void log_chat(int priority, const char* format, ...);
    static void log_player(int priority, const char* format, ...);
    static void log_vplayer(int priority, const char* format, ...);
    static void log_monster(int priority, const char* format, ...);
    static void log_robot(int priority, const char* format, ...);
    static void log_room(int priority, const char* format, ...);
    static void log_store(int priority, const char* format, ...);
    
};
#endif