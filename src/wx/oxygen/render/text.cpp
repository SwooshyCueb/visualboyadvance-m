#include "oxygen.h"
#include "text.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H

#include <glib.h>



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
    mmgmt = (FT_Memory)g_malloc(sizeof(struct FT_MemoryRec_));
    mmgmt->user = NULL;
    mmgmt->alloc = ft_malloc;
    mmgmt->free = ft_free;
    mmgmt->realloc = ft_realloc;

    if (fterr = FT_New_Library(mmgmt, &lib)) {
        errThrowVBAFT(VBA_ERR_FT_INIT, fterr);
    }

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

resTTF::resTTF() {
    is_init = FALSE;
}

resTTF::resTTF(gchar *ttf_in, const guint len) {
    init(ttf_in, len);
}

gboolean resTTF::init(gchar *ttf_in, const guint len) {
    ttf_main = (guchar *)ttf_in;
    ttf_len = len;
    is_init = TRUE;

    return TRUE;
}

resTTF::~resTTF() {
    // Nothing to do here
}

resTTF::resTTF(const resTTF &other) {
    ttf_main = other.ttf_main;
    ttf_len = other.ttf_len;
}

resTTF &resTTF::operator = (const resTTF &other) {
    ttf_main = other.ttf_main;
    ttf_len = other.ttf_len;
    return *this;
}

ftFont::ftFont() {
    is_init = FALSE;
    has_ttf = FALSE;
}

ftFont::ftFont(ftLib *lib) {
    init(lib);
}

gboolean ftFont::init(ftLib *lib) {
    ctx = lib;
    is_init = TRUE;
    has_ttf = FALSE;

    glyphSets = g_hash_table_new(g_direct_hash, g_direct_equal);
    return TRUE;
}

ftFont::ftFont(ftLib *lib, resTTF ttf_in) {
    init(lib, ttf_in);
}

gboolean ftFont::init(ftLib *lib, resTTF ttf_in) {
    return (init(lib) && setTTF(ttf_in));
}

// What do we do if we already have a font set?
gboolean ftFont::setTTF(resTTF ttf_in) {
    FT_Error fterr;
    ttf = ttf_in;

    if(fterr = FT_New_Memory_Face(ctx->lib, ttf.ttf_main, ttf.ttf_len, 0,
                                  &face)) {
        errThrowFT(fterr);
    }

    if(fterr = FT_Select_Charmap(face, ft_encoding_unicode)) {
        errThrowFT(fterr);
    }
    
    has_ttf = TRUE;
    return TRUE;
}

ftFont::~ftFont() {
    // Nothing to do yet
}

gboolean ftFont::setUndefinedIndex(guint32 idx) {
    undf = (gunichar)idx;
    return true;
}

gboolean ftFont::cacheGlyphs(guint size) {
    guint i;
    FT_Error fterr;
    //log_todo("Initial glyphset cache","Handle double-caches.");
    GHashTable *gset = g_hash_table_new(g_direct_hash, g_direct_equal);
    g_hash_table_insert(glyphSets, GUINT_TO_POINTER(size), (gpointer)gset);

    // Hardcoding DPI for now.
    if (fterr = FT_Set_Char_Size(face, 0, size*64, 96, 96)) {
        errThrowFT(fterr);
    }

    cacheGlyph((gunichar)undf, gset);

    for (i = 0x20; i <= 0x7E; i++) {
        cacheGlyph((gunichar)i, gset);
    }
    for (i = 0xa0; i <= 0x17E; i++) {
        cacheGlyph((gunichar)i, gset);
    }

    return true;
}

gboolean ftFont::cacheGlyph(gunichar character, GHashTable *gset) {
    ftGlyph *glyph;
    guint idx;
    //log_todo("Individual glyph cache", "Error if unable to load placeholder glyph.");
    gchar *u8c = (gchar *)g_malloc0(sizeof(gchar)*7);
    g_unichar_to_utf8(character, u8c);

    if(!(idx = FT_Get_Char_Index(face, character))) {
        log_debug("Glyph not loaded",
                  "%s glyph with value 0x%04ix "
                  "could not be loaded",
                  u8c,
                  (guint32)character);
        glyph = (ftGlyph *)g_hash_table_lookup(
            gset, GSIZE_TO_POINTER((gsize)character)
        );
    } else {
        FT_Load_Glyph(face, idx, FT_LOAD_RENDER);
        glyph = new ftGlyph;
        glyph->character = character;
        glyph->sz_tex.x = face->glyph->bitmap.width;
        glyph->sz_tex.y = face->glyph->bitmap.rows;
        glyph->sz_glyph.x = face->glyph->bitmap_left;
        glyph->sz_glyph.y = face->glyph->bitmap_top;
        glyph->adv = face->glyph->advance;

        glyph->data = new guchar[glyph->sz_tex.x * glyph->sz_tex.y * 4];
        memcpy(glyph->data, face->glyph->bitmap.buffer, glyph->sz_tex.x *
                                                        glyph->sz_tex.y * 4);

        log_debug("Glyph loaded",
                  "%s glyph info:\n"
                  "value: 0x%04ix\n"
                  "sz_tex: %u * %u\n"
                  "sz_glyph: %u * %u\n"
                  "adv: %u * %u",
                  u8c,
                  (guint32)character,
                  glyph->sz_tex.x, glyph->sz_tex.y,
                  glyph->sz_glyph.x, glyph->sz_glyph.y,
                  glyph->adv.x, glyph->adv.y);
    }
    g_hash_table_insert(gset, GSIZE_TO_POINTER((gsize)character),
                        (gpointer)glyph);
}

gboolean ftFont::cacheGlyph(gunichar character, guint size) {
    FT_Error fterr;
    // Hardcoding DPI for now.
    if (fterr = FT_Set_Char_Size(face, 0, size*64, 96, 96)) {
        errThrowFT(fterr);
    }

    // We will assume the hashsubtable has already been created
    GHashTable *gset;
    gset = (GHashTable *)g_hash_table_lookup(glyphSets,
                                             GUINT_TO_POINTER(size));
    return cacheGlyph(character, gset);

}

// This is probably not very efficient
ftGlyph *ftFont::getGlyph(gunichar character, guint size) {
    FT_Error fterr;
    // We will assume the hashsubtable has already been created
    GHashTable *gset;
    gset = (GHashTable *)g_hash_table_lookup(glyphSets,
                                             GUINT_TO_POINTER(size));

    if (!g_hash_table_contains(gset, GSIZE_TO_POINTER((gsize)character))) {
        if (fterr = FT_Set_Char_Size(face, 0, size*64, 96, 96)) {
            errThrowFT(fterr);
        }
        cacheGlyph(character, gset);
    }
    return (ftGlyph *)g_hash_table_lookup(gset,
                                          GSIZE_TO_POINTER((gsize)character));
}
