/***********************************************************************
 ***********************************************************************/
#ifndef _H_DEBUG_LOG
#define _H_DEBUG_LOG

extern unsigned int debug_mask;

#define debug_log(mask, format, args...) \
    if (mask & debug_mask) printf("%s - " format, __func__, ## args)

void debug_log_set_mask(unsigned int mask);

#endif
