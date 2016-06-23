// Not yet functional

#ifndef VBA_OSD_H
#define VBA_OSD_H

#include "swooshboy.h"
#include "vbagl.h"
#include "tex.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <deque>

class ftGlyph {
public:
    char character;

    vbaSize sz_tex;
    vbaSize sz_glyph;

    unsigned char *data;
    FT_Vector adv;
};

class vbaOSD {
    friend class vbaGL;
public:
    vbaOSD(vbaGL *globj);
    ~vbaOSD();

private:
    void getGlyph(char character);

    EH_DECLARE();

    bool init = false;

    struct {
        FT_Library lib;
        FT_Face face;
        FT_GlyphSlot gs;
    } ft;

    std::deque<ftGlyph> glyphs;
    ftGlyph currGlyph;
    ftGlyph placeholder;

    vbaGL *ctx;

};

#endif
