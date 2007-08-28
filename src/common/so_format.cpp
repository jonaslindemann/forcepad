#include "so_format.h"

static char g_buff[512];

#include <cstdarg>
#include <cstdio>

const char* so_format(char* format, ...)
{
	va_list args; 
    va_start(args, format); 
    vsprintf (g_buff, format, args); /* Format the string */

	return g_buff;
}