#include "swooshboy.h"
#include "osd.h"
#include "vbagl.h"
#include "tex.h"

#include <ft2build.h>
#include FT_FREETYPE_H

extern const unsigned char _binary_neoletters_ttf_start;
extern const unsigned char _binary_neoletters_ttf_end;

vbaOSD::vbaOSD(vbaGL *ctx) {
    FT_Error fterr;
    if (fterr = FT_Init_FreeType(&ft.lib)) {
        dprintf("Freetype lib init failed\n");
        errThrowVBAFT(VBA_ERR_FT_INIT, fterr);
    }

    FT_Long ttf_len = (FT_Long)(&_binary_neoletters_ttf_end - &_binary_neoletters_ttf_start);

    if (fterr = FT_New_Memory_Face(ft.lib, &_binary_neoletters_ttf_start, ttf_len, 0, &ft.face)) {
        dprintf("Freetype face init failed\n");
        errThrowVBAFT(VBA_ERR_FT_INIT, fterr);
    }

    if (fterr = FT_Set_Char_Size(ft.face, 0, 16*64, 96, 96)) {
        dprintf("Freetype size set failed\n");
        errThrowVBAFT(VBA_ERR_FT_INIT, fterr);
    }

    ft.gs = ft.face->glyph;

    init = true;
}

vbaOSD::~vbaOSD() {
    if (init) {
        FT_Done_Face(ft.face);
        FT_Done_FreeType(ft.lib);
    }
}
