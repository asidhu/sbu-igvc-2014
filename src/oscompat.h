#ifndef _OSCOMPATABILITY_H
#define _OSCOMPATABILITY_H
#include "basetypes.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif


uint64 getMicros();
#endif
