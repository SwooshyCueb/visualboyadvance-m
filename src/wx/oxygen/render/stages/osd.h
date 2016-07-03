// Not yet functional

#ifndef VBA_OSD_H
#define VBA_OSD_H

#include "oxygen.h"
#include "contrib/stb_rect_pack.h"
#include "contrib/stb_truetype.h"

class glyph {
    int advance;
    int leftSideBearing;
};

class stgOSD : public renderStage {
public:
    stgOSD(vbaGL *globj);
    bool init(vbaGL *globj);
    ~stgOSD();

    bool setIndex(uint idx, renderPipeline *rdrpth);
    bool render(vbaTex *src);

private:
    stbtt_pack_context packctx;
    #define NUM_GLYPHS 256
    #define ATLAS_GLYPH_S 24
    #define FONT_SIZE 16
    unsigned char atlaspx[NUM_GLYPHS * ATLAS_GLYPH_S * ATLAS_GLYPH_S];
    vbaTex tex_atlas;

    float scale;

    int vascent;
    int vdescent;
    int vlinegap;
    int vbaseline;

    stbtt_packedchar chardata[256];

    //static stbtt_pack_range char_ranges[];
};

#endif
