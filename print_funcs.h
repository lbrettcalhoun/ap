// Misc print and output functions for ap.c.  This header file defines the interfaces
// for the functions.  Extern is used to make it explicit that the functions are
// available to all other code in the program. Yeah, the defaut for a C function is 
// already "extern" but let's make it explicit in our declaration so we don't forget.

#include "c_types.h"
#include "user_interface.h"


#ifndef print_funcs_H
#define print_func_H

extern void ICACHE_FLASH_ATTR print_welcome(void);

extern void ICACHE_FLASH_ATTR print_config(struct softap_config *config);

#endif
