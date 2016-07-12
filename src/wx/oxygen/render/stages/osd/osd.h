// Not yet functional

#ifndef VBA_OSD_H
#define VBA_OSD_H

#include "oxygen.h"
#include "render/text.h"

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
    ftLib ft;
    ftFont fnt;

    #define NUM_GLYPHS 256
    #define ATLAS_GLYPH_S 24
    #define FONT_SIZE 16
    unsigned char atlaspx[NUM_GLYPHS * ATLAS_GLYPH_S * NUM_GLYPHS * ATLAS_GLYPH_S];
    vbaTex tex_atlas;
    GLuint vb_vtx;
    GLuint vb_texcoord;

    float scale;

    int vascent;
    int vdescent;
    int vlinegap;
    int vbaseline;


    //static stbtt_pack_range char_ranges[];

    struct {
        GLint texcoord = 0;
        GLint position = 0;
        GLint needs_flip = 0;
        GLint src_tex = 0;
        GLint is_passthrough = 0;
    } glsl_vars;
};

#endif
