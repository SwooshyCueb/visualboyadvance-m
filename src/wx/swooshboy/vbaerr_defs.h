/******************************************************************************/
/* err_defs.h                                                                 */
/******************************************************************************/
/* This header defines the actual error constants. It is not designed to be   */
/* included directly.                                                         */
/******************************************************************************/

//Standard errors

VBAERR_DEF_(OK,
            "No error",
            0x0000
)

VBAERR_DEF_(UNKN,
            "Unknown error",
            0x0001
)

VBAERR_DEF_(UNIMPL,
            "Unimplemented",
            0x0002
)

VBAERR_DEF_(NO_MORE,
            "No more data to fetch",
            0x0003
)

//GL-related errors

VBAERR_DEF_(GL_ERR,
            "Encountered a GL error",
            0x1000
)

VBAERR_DEF_(GL_INIT,
            "Encountered a problem during GL initialization",
            0x1001
)

//FreeType-related errors

VBAERR_DEF_(FT_ERR,
            "Encountered a FreeType error",
            0x2000
)

VBAERR_DEF_(FT_INIT,
            "Encountered a problem during FreeType initialization",
            0x2001
)
