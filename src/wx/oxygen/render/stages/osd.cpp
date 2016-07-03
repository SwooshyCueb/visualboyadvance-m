// Not yet functional

#include "oxygen.h"
#include "osd.h"

#include <glib.h>

#include "oxygen.h"
#include "contrib/stb_rect_pack.h"
#include "contrib/stb_truetype.h"

#define STAGE_MULT 1

stgOSD::stgOSD(vbaGL *globj)
    : renderStage(globj) {
    init(globj);
}

bool stgOSD::init(vbaGL *globj) {
    DECLARE_RES(ttf_neoletters);

    // We will eventually have our own shader for this.
    CREATE_GLSL_SRC_OBJ(pt_src, passthrough);

    glslShader shd_f(globj, GL_FRAGMENT_SHADER);
    glslShader shd_v(globj, GL_VERTEX_SHADER);
    shd_f.setSrc(&pt_src);
    shd_f.compile();
    shd_v.setSrc(&pt_src);
    shd_v.compile();

    shader.init(ctx);

    shader.attachShader(&shd_f);
    shader.attachShader(&shd_v);

    shader.link();

    setMult(STAGE_MULT);

    /* Might want to tweak this */
    stbtt_PackBegin(&packctx, atlaspx, (NUM_GLYPHS * ATLAS_GLYPH_S),
                    (NUM_GLYPHS * ATLAS_GLYPH_S), 0, 1, NULL);
    stbtt_PackFontRange(&packctx, (unsigned char*)res_ttf_neoletters, 0,
                         16.0f, 0x20, (0x100 - 0x20), chardata);

    stbtt_PackEnd(&packctx);

    tex_atlas.init(globj, (NUM_GLYPHS * ATLAS_GLYPH_S),
                   (NUM_GLYPHS * ATLAS_GLYPH_S), GL_ALPHA);
    tex_atlas.setData((GLvoid *)atlaspx);

    is_init = true;

}

stgOSD::~stgOSD() {
    if (is_init) {
        is_init = false;
    }
}

bool stgOSD::setIndex(uint idx, renderPipeline *rdrpth) {
    if (!is_init) {
        return false;
    }
    renderStage::setIndex(idx, rdrpth);
    shader.setSrcTexUnit(idx);
    shader.setNeedsFlip(false);
    has_shader = true;

    return true;
}

bool stgOSD::render(vbaTex *src) {
    if (!is_init) {
        return false;
    }

    char text[] = "This is a test, woo!\0";
    int tx, yx;
    tx = yx = 50;

    renderStage::render(src);

    return true;
}

