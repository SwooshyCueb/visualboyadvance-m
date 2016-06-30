#include "logging.h"
#include "swooshboy.h"


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

// TODO: Go ahead and have this function print instead of returning a queue
static GQueue *wwrap(const gchar *string, uint width) {
    GQueue *ret = g_queue_new();
    uint stringpos = 0;
    int elementpos = 0;
    uint backtrackpos;

    while (stringpos < strlen(string)) {
        // Allocate our queue element
        gchar *element = (gchar *)g_malloc0(width * sizeof(gchar));

        // Copy our string until the end of the line is reached
        for (elementpos = 0; elementpos < width-2; elementpos++) {

            // Check if we've hit the end of the string
            if (stringpos == strlen(string)) {
                // Push the last line to the queue and return
                g_queue_push_tail(ret, (gpointer)element);
                return ret;
            }
            element[elementpos] = string[stringpos];

            // Check if we've hit a newline
            if (element[elementpos] == '\n') {
                /* Push the element to the queue, make a new element, and
                 * reset the loop counter
                 */
                g_queue_push_tail(ret, (gpointer)element);
                element = (gchar *)g_malloc0(width * sizeof(gchar));
                elementpos = -1;
            }
            stringpos++;
        }
        // Do we need to increment elementpos here?

        // Did we get lucky?
        if (isspace(string[stringpos])) {
            // Add a newline and push the element to the queue
            element[elementpos] = '\n';
            g_queue_push_tail(ret, (gpointer)element);
            stringpos++;
        } else {
            // Find previous whitespace
            for(backtrackpos = elementpos; backtrackpos > 0; backtrackpos--) {
                if (isspace(element[backtrackpos])) {
                    element[backtrackpos] = '\n';
                    element[backtrackpos + 1] = '\0';
                    g_queue_push_tail(ret, (gpointer)element);
                    break;
                }
                stringpos--;
            }
            if (!backtrackpos) {
                stringpos += elementpos;
                element[elementpos+1] = '\n';
                element[elementpos+2] = '\0';
                g_queue_push_tail(ret, (gpointer)element);
            }
        }
    }
    return ret;
}

void logfunc_std(const gchar *levelstr, const gchar *title,
                 const gchar *message) {
    gchar *line, *timestr;
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

    GQueue *msg = wwrap(message, termwidth);
    while (g_queue_get_length(msg)) {
        line = (gchar *)g_queue_pop_head(msg);
        print("    %s", line);
        g_free(line);
    }
    print("\n\n");
    g_queue_free(msg);

}

void logfunc_dbg(const gchar *levelstr, const gchar *title,
                 const gchar *message, const gchar *fileline,
                 const gchar *func) {
    gchar *line, *timestr;
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

    GQueue *msg = wwrap(message, termwidth);
    while (g_queue_get_length(msg)) {
        line = (gchar *)g_queue_pop_head(msg);
        print("    %s", line);
        g_free(line);
    }
    print("\n    %s %s\n\n", func, fileline);
    g_queue_free(msg);

}

void logfunc_bkt(const gchar *levelstr, const gchar *title,
                 const gchar *message, const gchar *fileline,
                 const gchar *func, gpointer *traceaddrs, guint tracelen) {
    gchar *line, *timestr, **symbols, *tracefunc;
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

    GQueue *msg = wwrap(message, termwidth);
    while (g_queue_get_length(msg)) {
        line = (gchar *)g_queue_pop_head(msg);
        print("    %s", line);
        g_free(line);
    }
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
    g_queue_free(msg);

}
