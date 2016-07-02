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
#include <execinfo.h>
#include <cxxabi.h>
#include <cstdlib>

/* Our log flags and log levels have values that can be put together to form a
 * single value that can be passed around. In order to do this, the flag and
 * level values are defined by "1 << [number]".
 *
 * In order to avoid the potential human error that could come from manually
 * setting the values for our log flags and levels, we need to populate an enum
 * whose values we will later use to define the actual flag and level values.
 */

/* At this point in our journey, it is not necessary to differentiate between
 * flags and levels.
 */
#define VBA_LOG_PREENUM_(name) \
    VBA_LOG_SEQ_##name,

#define VBA_LOG_FLAG_DEF_(name) \
    VBA_LOG_PREENUM_(name)

#define VBA_LOG_LEVEL_DEF_(name, str, pad, c256, c16, c8, f256, f16,f8) \
    VBA_LOG_PREENUM_(name)

enum vba_logenum_seq {
    VBA_LOG_SEQ_INIT = 0,
    #include "logging_defs.h"
};

#undef VBA_LOG_FLAG_DEF_
#undef VBA_LOG_LEVEL_DEF_

/* Now, we can set the actual values for our flags and levels. */
#define VBA_LOG_FLAG_DEF_(name) \
    VBA_LOG_FLAG_##name = 1 << VBA_LOG_SEQ_##name,

#define VBA_LOG_LEVEL_DEF_(name, str, pad, c256, c16, c8, f256, f16,f8) \
    VBA_LOG_LEVEL_##name = 1 << VBA_LOG_SEQ_##name,

enum vba_loglevels {
    #include "logging_defs.h"

    #undef VBA_LOG_FLAG_DEF_
    #undef VBA_LOG_LEVEL_DEF_

    /* We also need a mask that can filter flags from provided values. */
    #define VBA_LOG_FLAG_DEF_(name) \
        VBA_LOG_FLAG_##name |
    #define VBA_LOG_LEVEL_DEF_(name, str, pad, c256, c16, c8, f256, f16,f8)

    VBA_LOG_LEVEL_MASK = ~(
        #include "logging_defs.h"
    0)
};

#undef VBA_LOG_FLAG_DEF_
#undef VBA_LOG_LEVEL_DEF_

/* Each log level has a string which is shown in the log output. Predictably,
 * this string tends to match the log level's name. This string is surrounded in
 * brackets and potential escape codes which result in different text formatting
 * in certian terminals. It may also be padded out to facilitate log
 * readability.
 *
 * Because we might or might not be outputing to a terminal that supports
 * the aformentioned escape codes, and because the level of support for these
 * escape codes can vary, we must define strings for each log level, for each
 * level of support. In order to do this, we declare a struct for containing
 * these strings. One for each log level, and some for any other bits of text
 * formatting we decide to do.
 */
#define VBA_LOG_FLAG_DEF_(name)
#define VBA_LOG_LEVEL_DEF_(name, str, pad, c256, c16, c8, f256, f16,f8) \
    gchar* VBA_LOG_LEVEL_##name##_STR;

struct vba_ll_str {
    #include "logging_defs.h"
    gchar *title_prefix;
    gchar *title_suffix;
};

#undef VBA_LOG_LEVEL_DEF_
#undef VBA_LOG_FLAG_DEF_

/* These variables will hold the strings for each level of support. */
extern struct vba_ll_str vba_loglevel_strings_256;
extern struct vba_ll_str vba_loglevel_strings_16;
extern struct vba_ll_str vba_loglevel_strings_8;
extern struct vba_ll_str vba_loglevel_strings_0;

/* This variable will contain the set of strings in use. */
extern struct vba_ll_str vba_loglevel_strings;

/* This variable holds the width of the terminal. */
extern uint termwidth;

#define TRACE_DEPTH 16
#define FUNCSTR_MAXLEN 76

#define _logline_std(levelstr, title, args...) \
    gchar *logstr; \
    logstr = g_strdup_printf(args); \
    logfunc_std(levelstr, title, logstr); \
    g_free(logstr)

#define _logline_dbg(levelstr, title, args...) \
    gchar *logstr, *fileline; \
    fileline = g_strdup_printf("%s:%d",basename(__FILE__), __LINE__); \
    logstr = g_strdup_printf(args); \
    logfunc_dbg(levelstr, title, logstr, fileline, __PRETTY_FUNCTION__); \
    g_free(fileline); \
    g_free(logstr)

#define _logline_bkt(levelstr, title, args...) \
    gchar *logstr, *fileline; \
    gpointer *traceaddrs = (gpointer *)g_malloc0(TRACE_DEPTH * sizeof(gpointer)); \
    guint tracelen = backtrace(traceaddrs, TRACE_DEPTH); \
    fileline = g_strdup_printf("%s:%d",basename(__FILE__), __LINE__); \
    logstr = g_strdup_printf(args); \
    logfunc_bkt(levelstr, title, logstr, fileline, __PRETTY_FUNCTION__, \
        traceaddrs, tracelen); \
    g_free(fileline); \
    g_free(logstr); \
    g_free(traceaddrs)

/* This makes inlusion of line-dependant "macros" easier. */
#define log_error(title, args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_ERROR) { \
            _logline_bkt(vba_loglevel_strings.VBA_LOG_LEVEL_ERROR_STR, \
                         title, args); \
            raise(SIGTRAP); \
        } \
    } while(0)
#define log_error_pass(title, args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_ERROR) { \
            _logline_bkt(vba_loglevel_strings.VBA_LOG_LEVEL_ERROR_STR, \
                         title, args); \
        } \
    } while(0)
#define log_critical(title, args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_CRITICAL) { \
            _logline_bkt(vba_loglevel_strings.VBA_LOG_LEVEL_CRITICAL_STR, \
                         title, args); \
        } \
    } while(0)
#define log_warning(title, args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_WARNING) { \
            _logline_std(vba_loglevel_strings.VBA_LOG_LEVEL_WARNING_STR, \
                         title, args); \
        } \
    } while(0)
#define log_message(title, args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_MESSAGE) { \
            _logline_std(vba_loglevel_strings.VBA_LOG_LEVEL_MESSAGE_STR, \
                         title, args); \
        } \
    } while(0)
#define log_warning_debug(title, args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_DEBUG_WARNING) { \
            _logline_dbg(vba_loglevel_strings.VBA_LOG_LEVEL_DEBUG_WARNING_STR, \
                         title, args); \
        } \
    } while(0)
#define log_info(title, args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_INFO) { \
            _logline_std(vba_loglevel_strings.VBA_LOG_LEVEL_INFO_STR, \
                         title, args); \
        } \
    } while(0)
#define log_debug(title, args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_DEBUG) { \
            _logline_dbg(vba_loglevel_strings.VBA_LOG_LEVEL_DEBUG_STR, \
                         title, args); \
        } \
    } while(0)
#define log_todo(title, args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_TODO) { \
            _logline_dbg(vba_loglevel_strings.VBA_LOG_LEVEL_TODO_STR, \
                         title, args); \
        } \
    } while(0)
#define log_fixme(title, args...) \
    do { \
        if (vbalog_enabled_levels & VBA_LOG_LEVEL_FIXME) { \
            _logline_dbg(vba_loglevel_strings.VBA_LOG_LEVEL_FIXME_STR, \
                         title, args); \
        } \
    } while(0)

/* This is our actual log function */
void logfunc_dbg(const gchar *levelstr, const gchar *title,
                 const gchar *message, const gchar *fileline,
                 const gchar *func);
void logfunc_bkt(const gchar *levelstr, const gchar *title,
                 const gchar *message, const gchar *fileline,
                 const gchar *func, gpointer *traceaddrs, guint tracelen);
void logfunc_std(const gchar *levelstr, const gchar *title,
                 const gchar *message);

#endif
