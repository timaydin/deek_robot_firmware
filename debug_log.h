/***********************************************************************
 ***********************************************************************/
#ifndef _H_DEBUG_LOG
#define _H_DEBUG_LOG

#define LOG_BUFFER_SIZE 64

#define debug_log(mask, format, args...) \
    _debug_log("%s - " format, __func__, ## args)

void _debug_log(const char* format, ...);

void debug_log_set_mask(unsigned int mask);

#endif
