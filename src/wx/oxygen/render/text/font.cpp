#include "oxygen.h"
#include "font.h"
#include "ftlib.h"
#include "glyph.h"
#include "ttfres.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glib.h>

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
        glyph->sz_tex.x(face->glyph->bitmap.width);
        glyph->sz_tex.y(face->glyph->bitmap.rows);
        glyph->sz_glyph.x(face->glyph->bitmap_left);
        glyph->sz_glyph.y(face->glyph->bitmap_top);
        glyph->adv = face->glyph->advance;

        glyph->data = new guchar[glyph->sz_tex.xu() * glyph->sz_tex.yu() * 4];
        memcpy(glyph->data, face->glyph->bitmap.buffer, glyph->sz_tex.xu() *
                                                        glyph->sz_tex.yu() * 4);

        /*
        guint i, j, p, c;
        guint bmst = (glyph->sz_tex.xu() * 3)+4;
        guint bmsz = bmst * glyph->sz_tex.yu() + 2;
        gchar *bitmap = (gchar *)g_malloc0(sizeof(gchar)*bmsz);
        if ((glyph->sz_tex.xu() * glyph->sz_tex.yu()) > 0) {
            for (i = 0; i < glyph->sz_tex.yu(); i++) {
                bitmap[i*bmst] = ' ';
                bitmap[(i*bmst)+1] = ' ';
                bitmap[(i*bmst)+2] = ' ';
                bitmap[(i*bmst)+3] = ' ';
                for(j = 0; j < glyph->sz_tex.xu(); j++) {
                    p = (i*bmst+(j*3))+4;
                    c = i*glyph->sz_tex.xu()+j;
                    g_snprintf(&bitmap[p], 4, "%.2x ", (uint)glyph->data[c]);
                }
                bitmap[(i+1)*bmst - 1] = '\n';
            }
            bitmap[bmsz-3] = '\0';
        }

        log_debug("Glyph loaded",
                  "%s glyph info:\n"
                  "value: 0x%04ix\n"
                  "sz_tex: %u * %u\n"
                  "offset: %d * %d\n"
                  "adv: %u * %u\n"
                  "data:\n%s",
                  u8c,
                  (guint32)character,
                  glyph->sz_tex.xu(), glyph->sz_tex.yu(),
                  glyph->sz_glyph.xi(), glyph->sz_glyph.yi(),
                  glyph->adv.x, glyph->adv.y,
                  bitmap);
        g_free(bitmap);
        g_free(u8c);
        */
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
