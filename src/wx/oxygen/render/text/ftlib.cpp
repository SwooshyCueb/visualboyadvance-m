#include "oxygen.h"
#include "ftlib.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H

#include <glib.h>

/* Let's use the glib versions of the memory management functions.
 * We will need to define functions for each to pass to FT_New_Library.
 * They cannot be methods of the ftLib class, as gcc will complain unless we
 * compile with -fpermissive, which I would like to avoid.
 */
static void *ft_malloc(FT_Memory, long sz) {
    return g_malloc(sz);
}

static void ft_free(FT_Memory, void *var) {
    g_free(var);
}

static void *ft_realloc(FT_Memory, long, long sz, void *var) {
    return g_realloc(var, sz);
}

ftLib::ftLib() {
    FT_Error fterr;

    // This is ugly, but we get segfaults if we don't do it
    mmgmt = (FT_Memory)g_malloc(sizeof(struct FT_MemoryRec_));

    mmgmt->user = NULL;
    mmgmt->alloc = ft_malloc;
    mmgmt->free = ft_free;
    mmgmt->realloc = ft_realloc;

    if (fterr = FT_New_Library(mmgmt, &lib)) {
        errThrowVBAFT(VBA_ERR_FT_INIT, fterr);
    }

    // Does not return an error value.
    FT_Add_Default_Modules(lib);
}

ftLib::~ftLib() {
    if (refcount == 1) {
        FT_Done_Library(lib);
    } else if (refcount > 1) {
        errThrowVBA(VBA_ERR_DEINIT_OBJ_IN_USE);
    }
}

void ftLib::ref() {
    vbaSharedObj::ref();
    FT_Reference_Library(lib);
}

void ftLib::unref() {
    FT_Done_Library(lib);
    vbaSharedObj::unref();
}
