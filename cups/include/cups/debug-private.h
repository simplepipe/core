/*
 * Private debugging macros for CUPS.
 *
 * Copyright © 2007-2018 by Apple Inc.
 * Copyright © 1997-2005 by Easy Software Products.
 *
 * Licensed under Apache License v2.0.  See the file "LICENSE" for more
 * information.
 */

#ifndef _CUPS_DEBUG_PRIVATE_H_
#  define _CUPS_DEBUG_PRIVATE_H_


/*
 * Include necessary headers...
 */

#  include <cups/versioning.h>


/*
 * C++ magic...
 */

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */


/*
 * The debug macros are used if you compile with DEBUG defined.
 *
 * Usage:
 *
 *   DEBUG_puts("string")
 *   DEBUG_printf(("format string", arg, arg, ...));
 *
 * Note the extra parenthesis around the DEBUG_printf macro...
 *
 * Newlines are not required on the end of messages, as both add one when
 * writing the output.
 *
 * If the first character is a digit, then it represents the "log level" of the
 * message from 0 to 9.  The default level is 1.  The following defines the
 * current levels we use:
 *
 * 0 = public APIs, other than value accessor functions
 * 1 = return values for public APIs
 * 2 = public value accessor APIs, progress for public APIs
 * 3 = return values for value accessor APIs
 * 4 = private APIs, progress for value accessor APIs
 * 5 = return values for private APIs
 * 6 = progress for private APIs
 * 7 = static functions
 * 8 = return values for static functions
 * 9 = progress for static functions
 *
 * The DEBUG_set macro allows an application to programmatically enable (or
 * disable) debug logging.  The arguments correspond to the CUPS_DEBUG_LOG,
 * CUPS_DEBUG_LEVEL, and CUPS_DEBUG_FILTER environment variables.
 */

#  ifdef DEBUG
#    define DEBUG_puts(x) _cups_debug_puts(x)
#    define DEBUG_printf(x) _cups_debug_printf x
#    define DEBUG_set(logfile,level,filter) _cups_debug_set(logfile,level,filter,1)
#  else
#    define DLLExport
#    define DEBUG_puts(x)
#    define DEBUG_printf(x)
#    define DEBUG_set(logfile,level,filter)
#  endif /* DEBUG */


/*
 * Prototypes...
 */

extern int	_cups_debug_fd _CUPS_PRIVATE;
extern int	_cups_debug_level _CUPS_PRIVATE;
extern void	_cups_debug_printf(const char *format, ...) _CUPS_FORMAT(1,2) _CUPS_PUBLIC;
extern void	_cups_debug_puts(const char *s) _CUPS_PUBLIC;
extern void	_cups_debug_set(const char *logfile, const char *level, const char *filter, int force) _CUPS_PUBLIC;
#  ifdef WIN32
extern int	_cups_gettimeofday(struct timeval *tv, void *tz) _CUPS_PRIVATE;
#    define gettimeofday(a,b) _cups_gettimeofday(a, b)
#  endif /* WIN32 */

#  ifdef __cplusplus
}
#  endif /* __cplusplus */

#endif /* !_CUPS_DEBUG_PRIVATE_H_ */
