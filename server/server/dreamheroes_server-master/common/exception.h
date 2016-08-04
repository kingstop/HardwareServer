#ifndef __exception_h__
#define __exception_h__
#include <boost/exception/all.hpp>
class MyException : public std::exception, public boost::exception
{
public:
	MyException(const char * str) ;
    ~MyException() throw();
	static void doneException(MyException& e);
	static void doneException(const char* fun,  unsigned int line);
	static void doneException(const char* str, const char* fun,  unsigned int line);
    virtual const char * what() const throw()
    {
        return _mywhats.c_str();
    }
protected:
	const char* _desc;
    const std::string _mywhats;
};
class MemoryException : public MyException
{
public:
	MemoryException(const char* str) : MyException(str)
	{
		_desc = "memory_exception";
	}
};
class BuffException : public MyException
{
public:
	BuffException(const char* str) : MyException(str)
	{
		_desc = "buff_exception";
	}
};
class SendException : public MyException
{
public:
	SendException(const char* str) : MyException(str)
	{
		_desc = "send_exception";
	}
};
class MyLuaException  : public MyException
{
public:
	MyLuaException(const char* str) : MyException(str)
	{
		_desc = "Lua Runtime error";
	}
};
class SimulatorException : public MyException
{
public:
    SimulatorException(const char* str) : MyException(str)
    {
        _desc = "Create Server Simulator error";
    }
};
class TCPNetException : public MyException
{
public:
    TCPNetException(const char* str) : MyException(str)
    {
        _desc = "tcp net exception";
    }
};

#define THROW_EXCEPTION(x) BOOST_THROW_EXCEPTION((x))
#define ENTER_FUN try{
#define LEAVE_FUN }catch(MyException& e){MyException::doneException(e);} \
    catch(std::exception& e){MyException::doneException(e.what(),__FUNCTION__, __LINE__);}      \
	catch(...){MyException::doneException(__FUNCTION__, __LINE__);}
#endif