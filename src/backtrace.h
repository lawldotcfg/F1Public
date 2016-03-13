#pragma once

// stuff for back tracing and getting a call stack (ideally)

#include <vector>
#include <exception>

class moduleException : public std::exception
{
public:
	explicit moduleException(const char* message) :
		msg_(message)
	{}

	explicit moduleException(const std::string& message) :
		msg_(message)
	{}

	virtual ~moduleException() throw () {}

	virtual const char *what() const throw ()
	{
		return msg_.c_str();
	}

protected:
	std::string msg_;
};

#define _TRYCATCH

// stuff for try and catch
// ugly but could improve speed later on
#ifdef _TRYCATCH
#define _TRY try
#undef _CATCH // undefine this _CATCH
#define _CATCH catch(...)
#define _CATCHMODULE catch(moduleException e)
#define _REPORT_ERROR(Class, funcName) \
	moduleException e(std::string("Error from ") + "\n" + Class->name() + "\n" + typeid(*Class).name() + "::" + #funcName); \
	throw e

// msvc SEH stuff
// TODO filters

class errState
{
public:
	static DWORD errCode;

	// TODO if this causes problems, change it to a small subset of instructions
	static int handler(int exceptCode)
	{
		// set our global state
		errState::errCode = exceptCode;
		// return that we want to handle this exception
		return 1;
	}
};

// try finally stuff for SEH
#define __TRY __try
#define __ERRCODE GetExceptionCode()
// default __except
#define __EXCEPT__(x) __except(x)
// handlerExcept
#define __EXCEPT __except(errState::handler(__ERRCODE))
#define __FINAL(x) __finally(x)

#else
#define _TRY
#define _CLOSE 
#define _CATCH 
#define _REPORT_ERROR(className,method)
#define _CATCHMODULE
#define _REPORT_ERROR

#define __TRY
#define __EXCEPT
#define __EXCEPT__
#define __FINAL
#endif