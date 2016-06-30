/* This is all very ugly, but I plan on making a solution similar to the one
 * used to define vbaErr values in order to restore code quality.
 */

#ifndef VBA_LOGGING_H
#define VBA_LOGGING_H

#include <glib.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <string.h>
#include <ctype.h>

// There's no reason to pretend I'm targeting anything but linux at this point
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>

enum vba_loglevels {
    VBA_LOG_FLAG_RECURSION          = G_LOG_FLAG_RECURSION,
    VBA_LOG_FLAG_FATAL              = G_LOG_FLAG_FATAL,
    VBA_LOG_FLAG_WARNING            = G_LOG_LEVEL_WARNING,

    VBA_LOG_LEVEL_ERROR             = G_LOG_LEVEL_ERROR,
    VBA_LOG_LEVEL_CRITICAL          = G_LOG_LEVEL_CRITICAL,
    VBA_LOG_LEVEL_MESSAGE           = G_LOG_LEVEL_MESSAGE,

    VBA_LOG_LEVEL_INFO              = G_LOG_LEVEL_INFO,
    VBA_LOG_LEVEL_DEBUG             = G_LOG_LEVEL_DEBUG,

    VBA_LOG_LEVEL_TODO              = 1 << 8,
    VBA_LOG_LEVEL_FIXME             = 1 << 9,

    VBA_LOG_LEVEL_MASK = ~(VBA_LOG_FLAG_RECURSION |
                           VBA_LOG_FLAG_FATAL |
                           VBA_LOG_FLAG_WARNING)
};

#define TERM256
/* Even though we do not currently try to detect the color output cabapilities
 * of the terminal, I went ahead and defined the escape codes for 8-color and
 * 16-color terminals.
 */
#define LOG_LF_RESET_ANY "\033[0m"
#define LOG_LF_BG_ANY "40"

#define LOG_LF_ERROR_COL_256 "38;5;160"
#define LOG_LF_ERROR_COL_16  "31"
#define LOG_LF_ERROR_COL_8   "31"
#define LOG_LF_ERROR_FMT_256 "1m"
#define LOG_LF_ERROR_FMT_8   "21m"
#define LOG_LF_ERROR_FMT_16  "21m"

#define LOG_LF_CRITICAL_COL_256 "38;5;214"
#define LOG_LF_CRITICAL_COL_16  "33"
#define LOG_LF_CRITICAL_COL_8   "31"
#define LOG_LF_CRITICAL_FMT_256 "1m"
#define LOG_LF_CRITICAL_FMT_8   "21m"
#define LOG_LF_CRITICAL_FMT_16  "21m"

#define LOG_LF_WARNING_COL_256 "38;5;227"
#define LOG_LF_WARNING_COL_16  "93"
#define LOG_LF_WARNING_COL_8   "33"
#define LOG_LF_WARNING_FMT_256 "1m"
#define LOG_LF_WARNING_FMT_8   "21m"
#define LOG_LF_WARNING_FMT_16  "21m"

#define LOG_LF_MESSAGE_COL_256 "38;5;69"
#define LOG_LF_MESSAGE_COL_16  "36"
#define LOG_LF_MESSAGE_COL_8   "36"
#define LOG_LF_MESSAGE_FMT_256 "21m"
#define LOG_LF_WARNING_FMT_8   "21m"
#define LOG_LF_WARNING_FMT_16  "21m"

#define LOG_LF_INFO_COL_256 "38;5;153"
#define LOG_LF_INFO_COL_16  "94"
#define LOG_LF_INFO_COL_8   "37"
#define LOG_LF_INFO_FMT_256 "21m"
#define LOG_LF_INFO_FMT_8   "21m"
#define LOG_LF_INFO_FMT_16  "21m"

#define LOG_LF_DEBUG_COL_256 "38;5;135"
#define LOG_LF_DEBUG_COL_16  "95"
#define LOG_LF_DEBUG_COL_8   "35"
#define LOG_LF_DEBUG_FMT_256 "21m"
#define LOG_LF_DEBUG_FMT_8   "21m"
#define LOG_LF_DEBUG_FMT_16  "21m"

#define LOG_LF_TODO_COL_256 "38;5;137"
#define LOG_LF_TODO_COL_16  "32"
#define LOG_LF_TODO_COL_8   "32"
#define LOG_LF_TODO_FMT_256 "1m"
#define LOG_LF_TODO_FMT_8   "21m"
#define LOG_LF_TODO_FMT_16  "21m"

#define LOG_LF_FIXME_COL_256 "38;5;173"
#define LOG_LF_FIXME_COL_16  "93"
#define LOG_LF_FIXME_COL_8   "33"
#define LOG_LF_FIXME_FMT_256 "1m"
#define LOG_LF_FIXME_FMT_8   "21m"
#define LOG_LF_FIXME_FMT_16  "21m"

#define LOG_LF_RESET LOG_LF_RESET_ANY

#if defined(TERM256)
#define LOG_LF_ERROR "\033[" LOG_LF_ERROR_COL_256 ";" LOG_LF_BG_ANY ";" LOG_LF_ERROR_FMT_256
#define LOG_LF_CRITICAL "\033[" LOG_LF_CRITICAL_COL_256 ";" LOG_LF_BG_ANY ";" LOG_LF_CRITICAL_FMT_256
#define LOG_LF_WARNING "\033[" LOG_LF_WARNING_COL_256 ";" LOG_LF_BG_ANY ";" LOG_LF_WARNING_FMT_256
#define LOG_LF_MESSAGE "\033[" LOG_LF_MESSAGE_COL_256 ";" LOG_LF_BG_ANY ";" LOG_LF_MESSAGE_FMT_256
#define LOG_LF_INFO "\033[" LOG_LF_INFO_COL_256 ";" LOG_LF_BG_ANY ";" LOG_LF_INFO_FMT_256
#define LOG_LF_DEBUG "\033[" LOG_LF_DEBUG_COL_256 ";" LOG_LF_BG_ANY ";" LOG_LF_DEBUG_FMT_256
#define LOG_LF_TODO "\033[" LOG_LF_TODO_COL_256 ";" LOG_LF_BG_ANY ";" LOG_LF_TODO_FMT_256
#define LOG_LF_FIXME "\033[" LOG_LF_FIXME_COL_256 ";" LOG_LF_BG_ANY ";" LOG_LF_FIXME_FMT_256

#elif defined(TERM16)
#define LOG_LF_ERROR "\033[" LOG_LF_ERROR_COL_16 ";" LOG_LF_BG_ANY ";" LOG_LF_ERROR_FMT_16
#define LOG_LF_CRITICAL "\033[" LOG_LF_CRITICAL_COL_16 ";" LOG_LF_BG_ANY ";" LOG_LF_CRITICAL_FMT_16
#define LOG_LF_WARNING "\033[" LOG_LF_WARNING_COL_16 ";" LOG_LF_BG_ANY ";" LOG_LF_WARNING_FMT_16
#define LOG_LF_MESSAGE "\033[" LOG_LF_MESSAGE_COL_16 ";" LOG_LF_BG_ANY ";" LOG_LF_MESSAGE_FMT_16
#define LOG_LF_INFO "\033[" LOG_LF_INFO_COL_16 ";" LOG_LF_BG_ANY ";" LOG_LF_INFO_FMT_16
#define LOG_LF_DEBUG "\033[" LOG_LF_DEBUG_COL_16 ";" LOG_LF_BG_ANY ";" LOG_LF_DEBUG_FMT_16
#define LOG_LF_TODO "\033[" LOG_LF_TODO_COL_16 ";" LOG_LF_BG_ANY ";" LOG_LF_TODO_FMT_16
#define LOG_LF_FIXME "\033[" LOG_LF_FIXME_COL_16 ";" LOG_LF_BG_ANY ";" LOG_LF_FIXME_FMT_16

#elif defined(TERM8)
#define LOG_LF_ERROR "\033[" LOG_LF_ERROR_COL_8 ";" LOG_LF_BG_ANY ";" LOG_LF_ERROR_FMT_8
#define LOG_LF_CRITICAL "\033[" LOG_LF_CRITICAL_COL_8 ";" LOG_LF_BG_ANY ";" LOG_LF_CRITICAL_FMT_8
#define LOG_LF_WARNING "\033[" LOG_LF_WARNING_COL_8 ";" LOG_LF_BG_ANY ";" LOG_LF_WARNING_FMT_8
#define LOG_LF_MESSAGE "\033[" LOG_LF_MESSAGE_COL_8 ";" LOG_LF_BG_ANY ";" LOG_LF_MESSAGE_FMT_8
#define LOG_LF_INFO "\033[" LOG_LF_INFO_COL_8 ";" LOG_LF_BG_ANY ";" LOG_LF_INFO_FMT_8
#define LOG_LF_DEBUG "\033[" LOG_LF_DEBUG_COL_8 ";" LOG_LF_BG_ANY ";" LOG_LF_DEBUG_FMT_8
#define LOG_LF_TODO "\033[" LOG_LF_TODO_COL_8 ";" LOG_LF_BG_ANY ";" LOG_LF_TODO_FMT_8
#define LOG_LF_FIXME "\033[" LOG_LF_FIXME_COL_8 ";" LOG_LF_BG_ANY ";" LOG_LF_FIXME_FMT_8

#endif

/* Might eventually be set at runtime */


/* We will be doing something different with this later */
#ifdef G_LOG_DOMAIN
#undef G_LOG_DOMAIN
#endif
#define G_LOG_DOMAIN "VBA"

#define _logline_std(level, args...) \
    g_log(G_LOG_DOMAIN, (GLogLevelFlags)level, "\n" args)

#define _logline_dbg(level, args...) \
    gchar *linestr, *logstr, *fullstr; \
    linestr = g_strdup_printf("%s:%d\n                %s\n",basename(__FILE__), \
        __LINE__,__PRETTY_FUNCTION__); \
    logstr  = g_strdup_printf(args); \
    fullstr = g_strconcat(linestr, logstr, NULL); \
    g_log(G_LOG_DOMAIN, (GLogLevelFlags)level, "%s", fullstr); \
    g_free(linestr); \
    g_free(logstr); \
    g_free(fullstr)

/* This makes inlusion of line-dependant "macros" easier. */
#define log_error(args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_ERROR) { \
            _logline_dbg(VBA_LOG_LEVEL_ERROR, args); \
        } \
    } while(0)
#define log_critical(args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_CRITICAL) { \
            _logline_dbg(VBA_LOG_LEVEL_CRITICAL, args); \
        } \
    } while(0)
#define log_warning_debug(args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_DEBUG) { \
            _logline_dbg(VBA_LOG_FLAG_WARNING, args); \
        } \
    } while(0)
#define log_warning(args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_MESSAGE) { \
            _logline_std(VBA_LOG_FLAG_WARNING, args); \
        } \
    } while(0)
#define log_message(args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_MESSAGE) { \
            _logline_std(VBA_LOG_LEVEL_MESSAGE, args); \
        } \
    } while(0)
#define log_info(args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_INFO) { \
            _logline_std(VBA_LOG_LEVEL_INFO, args); \
        } \
    } while(0)
#define log_debug(args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_DEBUG) { \
            _logline_dbg(VBA_LOG_LEVEL_DEBUG, args); \
        } \
    } while(0)
#define log_todo(args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_TODO) { \
            _logline_dbg(VBA_LOG_LEVEL_TODO, args); \
        } \
    } while(0)
#define log_fixme(args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_FIXME) { \
            _logline_dbg(VBA_LOG_LEVEL_FIXME, args); \
        } \
    } while(0)

/* This is our actual log function */
void logfunc_color(const gchar *domain, GLogLevelFlags level, const gchar *message,
              gpointer misc);
void logfunc_plain(const gchar *domain, GLogLevelFlags level, const gchar *message,
              gpointer misc);

#endif
