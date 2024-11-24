/***********************************************************************
 ***********************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include "console.h"
#include "debug_log.h"

unsigned int debug_mask = 1;

static char log_buffer[LOG_BUFFER_SIZE];

/***********************************************************************
 ***********************************************************************/
void _debug_log(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    int ret = vsprintf(log_buffer, format, args);
    if (ret >= 0)
    {
        unsigned int len = (unsigned int)ret;
        if (len >= sizeof(log_buffer))
        {
            len = sizeof(log_buffer);
        }

        console_write(log_buffer, len);
    }

    va_end(args);
}

/***********************************************************************
 ***********************************************************************/
void debug_log_set_mask(unsigned int mask)
{
    debug_mask = mask;
}
