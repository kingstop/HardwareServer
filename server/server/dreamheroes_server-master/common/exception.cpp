#include "exception.h"
#include "my_log.h"
MyException::MyException(const char * str)
	:std::exception(), _desc(""),_mywhats(str)
{
}
MyException::~MyException() throw()
{

}
void MyException::doneException(const char* str, const char* fun,  unsigned int line)
{
	Mylog::log_exception( LOG_ERROR, "Exception:[%s] Fun:[%s] Line:[%u]", str, fun, line);
}
void MyException::doneException(MyException& e)
{
	Mylog::log_exception( LOG_ERROR, "Exception:[%s] Desc:[%s] Fun:[%s] Line:[%u] FILE:[%s]", e.what(), e._desc,*boost::get_error_info<boost::throw_function>(e),
		 *boost::get_error_info<boost::throw_line>(e),  *boost::get_error_info<boost::throw_file>(e));
}
void MyException::doneException(const char* fun,  unsigned int line)
{
	Mylog::log_exception( LOG_ERROR, "Exception:Fun:[%s] Line:[%u]", fun, line);
}