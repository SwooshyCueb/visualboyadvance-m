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

VBAERR_DEF_(DEINIT_OBJ_IN_USE,
            "Attempted to deinitialize a shared object that was still in use",
            0x0004
)

// VBA-pipeline related errors

VBAERR_DEF_(UNSET_MULT,
            "Stage multiplier has not been set",
            0x0010
)

VBAERR_DEF_(UNSET_INDEX,
            "Stage index has not been set",
            0x0010
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
