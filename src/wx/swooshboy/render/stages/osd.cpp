// Not yet functional

#include "swooshboy.h"
#include "osd.h"

#include <ft2build.h>
#include FT_FREETYPE_H

vbaOSD::vbaOSD(vbaGL *globj) {
    DECLARE_RES(ttf_neoletters);
    ctx = globj;

    FT_Error fterr;
    if (fterr = FT_Init_FreeType(&ft.lib)) {
        errThrowVBAFT(VBA_ERR_FT_INIT, fterr);
    }

    if (fterr = FT_New_Memory_Face(ft.lib, (unsigned char*)res_ttf_neoletters, res_ttf_neoletters_len, 0, &ft.face)) {
        errThrowVBAFT(VBA_ERR_FT_INIT, fterr);
    }

    if (fterr = FT_Set_Char_Size(ft.face, 0, 16*64, 96, 96)) {
        errThrowVBAFT(VBA_ERR_FT_INIT, fterr);
    }

    if (fterr = FT_Load_Char(ft.face, '�', FT_LOAD_RENDER)) {
        errThrowVBAFT(VBA_ERR_FT_INIT, fterr);
    }

    ft.gs = ft.face->glyph;


    placeholder.character = '�';
    placeholder.sz_tex = vbaSize(ft.gs->bitmap.width, ft.gs->bitmap.rows);
    placeholder.sz_glyph = vbaSize(ft.gs->bitmap_left, ft.gs->bitmap_top);
    placeholder.adv = ft.gs->advance;
    log_debug("Placeholder glyph info",
              "sz_tex: %u * %u\n"
              "sz_glyph: %u * %u\n",
              "adv: %u * %u",
              placeholder.sz_tex.x, placeholder.sz_tex.y,
              placeholder.sz_glyph.x, placeholder.sz_glyph.y,
              placeholder.adv.x, placeholder.adv.y);

    placeholder.data = new unsigned char[placeholder.sz_tex.x *
                                         placeholder.sz_tex.y * 4];
    memcpy(placeholder.data, ft.gs->bitmap.buffer,
           placeholder.sz_tex.x * placeholder.sz_tex.y * 4);

    glyphs.push_back(placeholder);

    init = true;
    // Preload some glyphs here?

}

vbaOSD::~vbaOSD() {
    if (init) {
        FT_Done_Face(ft.face);
        FT_Done_FreeType(ft.lib);
    }
}

EH_DEFINE(vbaOSD);

void vbaOSD::getGlyph(char character) {
    std::deque<ftGlyph>::iterator iter = glyphs.begin();
    while (iter != glyphs.end()) {
        if (iter->character == character) {
            currGlyph = *iter;
            return;
        }
        iter++;
    }

    ftGlyph ng;
    ng.character = character;

    if (FT_Load_Char(ft.face, character, FT_LOAD_RENDER)) {
        ng.character = character;
        ng.sz_tex = placeholder.sz_tex;
        ng.sz_glyph = placeholder.sz_glyph;
        ng.adv = placeholder.adv;
        ng.data = placeholder.data;
        return;
    } else {
        ng.sz_tex = vbaSize(ft.gs->bitmap.width, ft.gs->bitmap.rows);
        ng.sz_glyph = vbaSize(ft.gs->bitmap_left, ft.gs->bitmap_top);
        ng.adv = ft.gs->advance;

        ng.data = new unsigned char[ng.sz_tex.x * ng.sz_tex.y * 4];
        memcpy(ng.data, ft.gs->bitmap.buffer, ng.sz_tex.x * ng.sz_tex.y * 4);
    }

    log_debug("%c glyph info:",
              "sz_tex: %u * %u\n"
              "sz_glyph: %u * %u\n",
              "adv: %u * %u",
              character,
              ng.sz_tex.x, ng.sz_tex.y,
              ng.sz_glyph.x, ng.sz_glyph.y,
              ng.adv.x, ng.adv.y);

    currGlyph = ng;

    glyphs.push_back(ng);
}
