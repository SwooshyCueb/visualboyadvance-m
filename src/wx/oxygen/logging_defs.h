
/* VBA_LOG_FLAG_DEF_(flag_name)
 */

VBA_LOG_FLAG_DEF_(RECURSION)

VBA_LOG_FLAG_DEF_(FATAL)

/* VBA_LOG_LEVEL_DEF_(level_name,
 *                    level_string,
 *                    level_string_padding,
 *                    level_string_color_256,
 *                    level_string_color_16,
 *                    level_string_color_8
 *                    level_string_format_256,
 *                    level_string_format_16,
 *                    level_string_format_8
 * )
 */

VBA_LOG_LEVEL_DEF_(ERROR,
                   "[ERROR]",
                   "   ",
                   "38;5;160",
                   "31",
                   "31",
                   "1",
                   "21",
                   "21"
)

VBA_LOG_LEVEL_DEF_(CRITICAL,
                   "[CRITICAL]",
                   "",
                   "38;5;214",
                   "33",
                   "31",
                   "1",
                   "21",
                   "21"
)

VBA_LOG_LEVEL_DEF_(WARNING,
                   "[WARNING]",
                   " ",
                   "38;5;227",
                   "93",
                   "33",
                   "1",
                   "21",
                   "21"
)

VBA_LOG_LEVEL_DEF_(MESSAGE,
                   "[MESSAGE]",
                   " ",
                   "38;5;69",
                   "36",
                   "36",
                   "21",
                   "21",
                   "21"
)

VBA_LOG_LEVEL_DEF_(DEBUG_WARNING,
                   "[WARNING]",
                   " ",
                   "38;5;227",
                   "93",
                   "33",
                   "1",
                   "21",
                   "21"
)

VBA_LOG_LEVEL_DEF_(INFO,
                   "[INFO]",
                   "    ",
                   "38;5;153",
                   "94",
                   "37",
                   "21",
                   "21",
                   "21"
)


VBA_LOG_LEVEL_DEF_(DEBUG,
                   "[DEBUG]",
                   "   ",
                   "38;5;135",
                   "95",
                   "35",
                   "21",
                   "21",
                   "21"
)


VBA_LOG_LEVEL_DEF_(TODO,
                   "[TODO]",
                   "    ",
                   "38;5;137",
                   "32",
                   "32",
                   "1",
                   "21",
                   "21"
)


VBA_LOG_LEVEL_DEF_(FIXME,
                   "[FIXME]",
                   "   ",
                   "38;5;173",
                   "93",
                   "33",
                   "1",
                   "21",
                   "21"
)
