#include "backTrace.h"

void sehTranslator(unsigned int code, EXCEPTION_POINTERS *e)
{
	// create a new exception base off of these and then throw it.
	//Log::Error("preparing to throw seh exception!");
	throw sehException(code, e);
}
