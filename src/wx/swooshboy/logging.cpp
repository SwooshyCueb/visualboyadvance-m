#include "swooshboy.h"
#include "logging.h"


GLogLevelFlags vbalog_enabled_levels = (GLogLevelFlags)(
    VBA_LOG_LEVEL_ERROR |
    VBA_LOG_LEVEL_CRITICAL |
    VBA_LOG_LEVEL_MESSAGE |
//    #ifdef _DEBUG
    VBA_LOG_LEVEL_INFO |
    VBA_LOG_LEVEL_DEBUG
            |
//    #else
    VBA_LOG_LEVEL_TODO |
    VBA_LOG_LEVEL_FIXME
//    #endif
);

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

void logfunc_color(const gchar *domain, GLogLevelFlags level, const gchar *message,
             gpointer misc)
{
    gchar *line;
    struct winsize *termsz = (struct winsize *)misc;
    void (*print)(const gchar*, ...);
    // Until we rid ourselves of wx, stdout is pretty useless
    print = g_printerr;
    GDateTime *time = g_date_time_new_now_local();
    gchar *timestr = g_date_time_format(time,"%T");
    g_date_time_unref(time);
    print("%s ", timestr);
    g_free(timestr);
    if (level & VBA_LOG_FLAG_WARNING) {
        print(" " LOG_LF_WARNING "[WARNING]");
    } else {
        switch(level & (VBA_LOG_LEVEL_MASK))
        {
            case VBA_LOG_LEVEL_ERROR:
                print("   " LOG_LF_ERROR "[ERROR]");
                break;
            case VBA_LOG_LEVEL_CRITICAL:
                print(LOG_LF_CRITICAL "[CRITICAL]");
                break;
            case VBA_LOG_LEVEL_MESSAGE:
                print(" " LOG_LF_MESSAGE "[MESSAGE]");
                break;
            case VBA_LOG_LEVEL_INFO:
                print("    " LOG_LF_INFO "[INFO]");
                break;
            case VBA_LOG_LEVEL_DEBUG:
                print("   " LOG_LF_DEBUG "[DEBUG]");
                break;
            case VBA_LOG_LEVEL_TODO:
                print("    " LOG_LF_TODO "[TODO]");
                break;
            case VBA_LOG_LEVEL_FIXME:
                print("   " LOG_LF_FIXME "[FIXME]");
                break;
        }
    }
    GQueue *msg = wwrap(message, termsz->ws_col - 4);
    line = (gchar *)g_queue_pop_head(msg);
    print(LOG_LF_RESET " %s", line);
    g_free(line);
    while (g_queue_get_length(msg)) {
        line = (gchar *)g_queue_pop_head(msg);
        print("    %s", line);
        g_free(line);
    }
    print("\n\n");
    g_queue_free(msg);
}


void logfunc_plain(const gchar *domain, GLogLevelFlags level, const gchar *message,
             gpointer misc)
{
    gchar *line;
    void (*print)(const gchar*, ...);
    // Until we rid ourselves of wx, stdout is pretty useless
    print = g_printerr;
    GDateTime *time = g_date_time_new_now_local();
    gchar *timestr = g_date_time_format(time,"%T");
    g_date_time_unref(time);
    print("%s ", timestr);
    g_free(timestr);
    if (level & VBA_LOG_FLAG_WARNING) {
        print(" [WARNING]");
    } else {
        switch(level & (VBA_LOG_LEVEL_MASK))
        {
            case VBA_LOG_LEVEL_ERROR:
                print("   [ERROR]");
                break;
            case VBA_LOG_LEVEL_CRITICAL:
                print("[CRITICAL]");
                break;
            case VBA_LOG_LEVEL_MESSAGE:
                print(" [MESSAGE]");
                break;
            case VBA_LOG_LEVEL_INFO:
                print("    [INFO]");
                break;
            case VBA_LOG_LEVEL_DEBUG:
                print("   [DEBUG]");
                break;
            case VBA_LOG_LEVEL_TODO:
                print("    [TODO]");
                break;
            case VBA_LOG_LEVEL_FIXME:
                print("   [FIXME]");
                break;
        }
    }
    GQueue *msg = wwrap(message, 76);
    line = (gchar *)g_queue_pop_head(msg);
    print(" %s", line);
    g_free(line);
    while (g_queue_get_length(msg)) {
        line = (gchar *)g_queue_pop_head(msg);
        print("    %s", line);
        g_free(line);
    }
    print("\n\n");
    g_queue_free(msg);
}
