// Not yet functional

/* TODO:
 * "Statusbar" text. (FPS, speed %, and such)
 *     does not disappear on its own
 * "Console"-type scrolling text
 *     starts in top right corner, is pushed down to make room for new text,
 *     rather than being replaced by new text. Stays on screen for a little
 *     while before disappearing (fade out?)
 * Unicode support
 */

#ifndef VBA_OSD_H
#define VBA_OSD_H

#include "oxygen.h"
#include "render/text/text.h"
#include <deque>

class osdLine {
public:
    vbaTex tex;
    GLuint buffer;
    vbaSize pos;
    gint timeout = -60 * 5;
};

class stgOSD : public renderStage {
public:
    stgOSD(vbaGL *globj);
    bool init(vbaGL *globj);
    ~stgOSD();

    bool setIndex(uint idx, renderPipeline *rdrpth);
    bool render(vbaTex *src);

    bool pushText(gunichar *text);
    bool setSpeed(gfloat fps, gfloat speed);

private:
    bool renderGlyph(gunichar character, vbaSize pos);

    ftLib ft;
    ftFont fnt;

    GLuint vb_vtx;
    GLuint vb_texcoord;
    glslProg shd_glyph;

    std::deque<osdLine*> scroll;

    vbaTex tex_glyph;
    vbaTex tex_stats;
    vbaTex tex_osd;

    float scale;

    int vascent;
    int vdescent;
    int vlinegap;
    int vbaseline;
    vbaSize sz_texel;


    //static stbtt_pack_range char_ranges[];

    struct {
        GLint texcoord = 0;
        GLint position = 0;
        GLint needs_flip = 0;
        GLint src_tex = 0;
        //GLint is_passthrough = 0;
    } p_glsl_vars;

    struct {
        GLint texcoord = 0;
        GLint position = 0;
        GLint needs_flip = 0;
        GLint src_tex = 0;
        GLint fg_color = 0;
        GLint bg_color = 0;
    } g_glsl_vars;
};

#endif
