#include "logging.h"
#include "oxygen.h"


struct vba_ll_str vba_loglevel_strings;

#define VBA_LOG_FLAG_DEF_(name)


#define VBA_LOG_LEVEL_DEF_(name, str, pad, c256, c16, c8, f256, f16,f8) \
   .VBA_LOG_LEVEL_##name##_STR = \
        pad "\033[" c256 ";40;" f256 "m" str "\033[0m",
struct vba_ll_str vba_loglevel_strings_256 = {
    #include "logging_defs.h"
    .title_prefix = "\033[1m",
    .title_suffix = "\033[21m"
};
#undef VBA_LOG_LEVEL_DEF_

#define VBA_LOG_LEVEL_DEF_(name, str, pad, c256, c16, c8, f256, f16,f8) \
    .VBA_LOG_LEVEL_##name##_STR = \
        pad "\033[" c16 ";40;" f16 "m" str "\033[0m",
struct vba_ll_str vba_loglevel_strings_16 = {
    #include "logging_defs.h"
    .title_prefix = "\033[1m",
    .title_suffix = "\033[21m"
};
#undef VBA_LOG_LEVEL_DEF_

#define VBA_LOG_LEVEL_DEF_(name, str, pad, c256, c16, c8, f256, f16,f8) \
    .VBA_LOG_LEVEL_##name##_STR = \
        pad "\033[" c8 ";40;" f8 "m" str "\033[0m",
struct vba_ll_str vba_loglevel_strings_8 = {
    #include "logging_defs.h"
    .title_prefix = "\033[1m",
    .title_suffix = "\033[21m"
};
#undef VBA_LOG_LEVEL_DEF_

#define VBA_LOG_LEVEL_DEF_(name, str, pad, c256, c16, c8, f256, f16,f8) \
    .VBA_LOG_LEVEL_##name##_STR = \
        pad str,
struct vba_ll_str vba_loglevel_strings_0 = {
    #include "logging_defs.h"
    .title_prefix = "",
    .title_suffix = ""
};
#undef VBA_LOG_LEVEL_DEF_

uint termwidth;
#undef VBA_LOG_LEVEL_DEF_
#undef VBA_LOGGING_DEFS

GLogLevelFlags vbalog_enabled_levels = (GLogLevelFlags)(
    VBA_LOG_LEVEL_ERROR |
    VBA_LOG_LEVEL_CRITICAL |
    VBA_LOG_LEVEL_WARNING |
    VBA_LOG_LEVEL_MESSAGE |
    #ifdef _DEBUG
    VBA_LOG_LEVEL_DEBUG_WARNING |
    VBA_LOG_LEVEL_INFO |
    VBA_LOG_LEVEL_DEBUG |
    #endif
    VBA_LOG_LEVEL_TODO |
    VBA_LOG_LEVEL_FIXME
);

void wwrap(const gchar *string, uint width, gpointer printfunc) {
    uint stringpos = 0;
    int linepos = 0;
    uint backtrackpos, twidth;
    if (width) {
        twidth = width;
    } else {
        twidth = 80;
    }

    void (*print)(const gchar*, ...);
    print = (void (*)(const gchar*, ...))printfunc;

    while (stringpos < strlen(string)) {
        // Allocate our line string
        gchar *line = (gchar *)g_malloc0(twidth * sizeof(gchar));

        // Copy our string until the end of the line is reached
        for (linepos = 0; linepos < (int)twidth-2; linepos++) {

            // Check if we've hit the end of the string
            if (stringpos == strlen(string)) {
                // Print the last line and return
                print("    %s", line);
                g_free(line);
                return;
            }
            line[linepos] = string[stringpos];

            // Check if we've hit a newline
            if (line[linepos] == '\n') {
                /* Print the line, reallocate the string, and reset the loop
                 * counter
                 */
                print("    %s", line);
                g_free(line);
                line = (gchar *)g_malloc0(twidth * sizeof(gchar));
                linepos = -1;
            }
            stringpos++;
        }

        // Did we get lucky?
        if (isspace(string[stringpos])) {
            // Change whitespace to newline, print, and free
            line[linepos] = '\n';
            print("    %s", line);
            g_free(line);
            stringpos++;
        } else {
            // Find previous whitespace
            for(backtrackpos = linepos; backtrackpos > 0; backtrackpos--) {
                if (isspace(line[backtrackpos])) {
                    line[backtrackpos] = '\n';
                    line[backtrackpos + 1] = '\0';
                    print("    %s", line);
                    g_free(line);
                    break;
                }
                stringpos--;
            }
            if (!backtrackpos) {
                stringpos += linepos;
                line[linepos+1] = '\n';
                line[linepos+2] = '\0';
                print("    %s", line);
                g_free(line);
            }
        }
    }
    return;
}

void logfunc_std(const gchar *levelstr, const gchar *title,
                 const gchar *message) {
    gchar *timestr;
    GDateTime *time;

    void (*print)(const gchar*, ...);
    // Until we rid ourselves of wx, stdout is pretty useless
    print = g_printerr;

    time = g_date_time_new_now_local();
    timestr = g_date_time_format(time,"%T");

    print("%s %s %s%s%s\n", timestr, levelstr,
          vba_loglevel_strings.title_prefix, title,
          vba_loglevel_strings.title_suffix);

    g_date_time_unref(time);
    g_free(timestr);

    wwrap(message, termwidth, (gpointer)print);
    print("\n\n");

}

void logfunc_dbg(const gchar *levelstr, const gchar *title,
                 const gchar *message, const gchar *fileline,
                 const gchar *func) {
    gchar *timestr;
    GDateTime *time;

    void (*print)(const gchar*, ...);
    // Until we rid ourselves of wx, stdout is pretty useless
    print = g_printerr;

    time = g_date_time_new_now_local();
    timestr = g_date_time_format(time,"%T");

    print("%s %s %s%s%s\n", timestr, levelstr,
          vba_loglevel_strings.title_prefix, title,
          vba_loglevel_strings.title_suffix);

    g_date_time_unref(time);
    g_free(timestr);

    wwrap(message, termwidth, (gpointer)print);
    print("\n    %s %s\n\n", func, fileline);

}

void logfunc_bkt(const gchar *levelstr, const gchar *title,
                 const gchar *message, const gchar *fileline,
                 const gchar *func, gpointer *traceaddrs, guint tracelen) {
    gchar *timestr, **symbols, *tracefunc;
    GDateTime *time;
    size_t tracefunclen = FUNCSTR_MAXLEN;

    void (*print)(const gchar*, ...);
    // Until we rid ourselves of wx, stdout is pretty useless
    print = g_printerr;

    time = g_date_time_new_now_local();
    timestr = g_date_time_format(time,"%T");

    print("%s %s %s%s%s\n", timestr, levelstr,
          vba_loglevel_strings.title_prefix, title,
          vba_loglevel_strings.title_suffix);

    g_date_time_unref(time);
    g_free(timestr);

    wwrap(message, termwidth, (gpointer)print);
    print("\n");
    print("    Stacktrace:\n");
    print("        %s %s\n", func, fileline);

    symbols = backtrace_symbols(traceaddrs, tracelen);
    tracefunc = (gchar *)g_malloc0(tracefunclen * sizeof(gchar));
    for (guint i = 1; i < tracelen; i++) {
        gchar *begin_name = 0, *begin_offset = 0, *end_offset = 0;
        for (char *p = symbols[i]; *p; ++p) {
            if (*p == '(') {
                begin_name = p;
            } else if (*p == '+') {
                begin_offset = p;
            } else if (*p == ')') {
                end_offset = p;
                break;
            }
        }
        if (begin_name &&
            begin_offset &&
            end_offset &&
            (begin_name < begin_offset)) {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';

            gint status;
            gchar *tracefunc2 = abi::__cxa_demangle(begin_name, tracefunc,
                                                    &tracefunclen, &status);
            print("        ");
            if (strcmp(symbols[i], symbols[1]) != 0) {
                if (status == 0) {
                    tracefunc = tracefunc2;
                }
                continue;
                //print("%s:", basename(symbols[i]));
            }
            if (status == 0) {
                tracefunc = tracefunc2;

                print("%s+%s\n", tracefunc, begin_offset);
            } else {
                print("%s()+%s\n", begin_name, begin_offset);
            }
        } else {
            print("%s\n", basename(symbols[i]));
        }
    }
    g_free(symbols);
    g_free(tracefunc);

    print("\n");

}
