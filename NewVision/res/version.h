// version.h
#include "version.ver"
#define		_STR(x) #x
#define		STR(x) _STR(x)
#define		VERSION_NUMBER			VERSION_MAJOR,VERSION_MINOR,VERSION_BUILD,VERSION_QFE
#define		VERSION_STRING			STR(VERSION_MAJOR) "." STR(VERSION_MINOR) "." STR(VERSION_BUILD) "." STR(VERSION_QFE)
#define		VERSION_COMPANY			""
#define		VERSION_COPYRIGHT		"(C) Alex Leykin 2005-2006"
#define		VERSION_TRADEMARK		""
#define		VERSION_BUILD_DATE_TIME	VERSION_BUILD_DATE " - " VERSION_BUILD_TIME
