#ifndef VBA_OSD_H
#define VBA_OSD_H

#include "swooshboy.h"
#include "vbagl.h"
#include "tex.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class vbaOSD {
    friend class vbaGL;
public:
    vbaOSD(vbaGL *ctx);
    ~vbaOSD();

private:
    bool init = false;

    struct {
        FT_Library lib;
        FT_Face face;
        FT_GlyphSlot gs;
    } ft;

};

#endif
