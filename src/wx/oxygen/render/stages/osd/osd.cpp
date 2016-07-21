// Not yet functional

#include "oxygen.h"
#include "osd.h"

#include <glib.h>

#include "oxygen.h"
#include "render/text/text.h"

#include <vector>

#define STAGE_MULT 1

#define FONT_SIZE 12

stgOSD::stgOSD(vbaGL *globj)
    : renderStage(globj) {
    init(globj);
}

bool stgOSD::init(vbaGL *globj) {
    //DECLARE_RES(ttf_neoletters);
    //DECLARE_RES(ttf_o2);

    CREATE_TTF_RES_OBJ(res_neoletters, neoletters);
    fnt.init(&ft, res_neoletters);
    fnt.cacheGlyphs(FONT_SIZE);

    // We will eventually have our own shader for this.
    CREATE_GLSL_SRC_OBJ(osd_glsl, osd);
    //CREATE_GLSL_SRC_OBJ(osd_glsl, passthrough);

    glslShader shd_f(globj, GL_FRAGMENT_SHADER);
    glslShader shd_v(globj, GL_VERTEX_SHADER);
    shd_f.setSrc(&osd_glsl);
    shd_f.compile();
    shd_v.setSrc(&osd_glsl);
    shd_v.compile();

    shader.init(ctx);

    shader.attachShader(&shd_f);
    shader.attachShader(&shd_v);

    shader.link();

    glsl_vars.position = shader.getAttrPtr("v_pos");
    glsl_vars.texcoord = shader.getAttrPtr("v_texcoord");
    glsl_vars.src_tex = shader.getUniformPtr("src_tex");
    glsl_vars.needs_flip = shader.getUniformPtr("needs_flip");
    glsl_vars.is_passthrough = shader.getUniformPtr("is_passthrough");

    setMult(STAGE_MULT);

    shader.setVar1i(glsl_vars.needs_flip, 0);
    shader.setVar1i(glsl_vars.is_passthrough, 1);

    shader.activate();
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
    shader.setVtxAttrPtr(glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
    shader.setVtxAttrPtr(glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    tex_stats.init(globj, getSize(), GL_ALPHA);
    tex_osd.init(globj, getSize(), GL_ALPHA);

    // Size here doesn't matter
    tex_glyph.init(globj, 4, 4, GL_ALPHA);
    tex_glyph.bind(0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    //tex_atlas.init(globj, (NUM_GLYPHS * ATLAS_GLYPH_S),
    //               (NUM_GLYPHS * ATLAS_GLYPH_S), GL_ALPHA);
    //tex_atlas.setData((GLvoid *)atlaspx);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &vb_vtx);
    glGenBuffers(1, &vb_texcoord);


    is_init = true;

}

stgOSD::~stgOSD() {
    if (is_init) {
        is_init = false;
    }
}

bool stgOSD::pushText(gchar *text) {
    // Create texture for new line of text

    // Change existing textures' positions

    // Remove any textures pushed below midpoint of screen

    return true;
}

bool stgOSD::setSpeed(gfloat fps, gfloat speed) {
    // Clear current speed texture

    // Render new speed texture

    return true;
}

bool stgOSD::setIndex(uint idx, renderPipeline *rdrpth) {
    if (!is_init) {
        return false;
    }
    renderStage::setIndex(idx, rdrpth);
    shader.setVar1i(glsl_vars.src_tex, (GLint)idx);
    has_shader = true;
    sz_texel = vbaSize(2.0/getSize());

    return true;
}

bool stgOSD::renderGlyph(gunichar character, vbaSize pos) {
    ftGlyph *g = fnt.getGlyph(character, FONT_SIZE);

    // we need to bring down the number of glTexImage2D calls
    tex_glyph.setSize(g->sz_tex - vbaSize(1,1));
    tex_glyph.setData((GLvoid *)g->data);
    tex_glyph.bind(0);

    vbaSize ioff = vbaSize(g->sz_glyph.xd(),
                           g->sz_glyph.yd() - g->sz_tex.yd());
    vbaSize ipos = pos + ioff;
    vbaSize fpos = (ipos * sz_texel) - 1.0;
    vbaSize fend = ((ipos + (g->sz_tex)) * sz_texel) - 1.0;

    GLfloat tpos[4][2] = {
        {fpos.xf(), fpos.yf()},
        {fend.xf(), fpos.yf()},
        {fend.xf(), fend.yf()},
        {fpos.xf(), fend.yf()}
    };

    shader.activate();
    shader.setVar1i(glsl_vars.is_passthrough, 0);
    shader.setVar1i(glsl_vars.src_tex, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vb_vtx);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tpos, GL_DYNAMIC_DRAW);
    shader.setVtxAttrPtr(glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, buffer);

    glEnable(GL_BLEND);

    shader.enableVertAttrArr(glsl_vars.position);
    shader.enableVertAttrArr(glsl_vars.texcoord);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    shader.disableVertAttrArr(glsl_vars.position);
    shader.disableVertAttrArr(glsl_vars.texcoord);

    glDisable(GL_BLEND);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
    shader.setVtxAttrPtr(glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shader.setVar1i(glsl_vars.src_tex, index);
    shader.setVar1i(glsl_vars.is_passthrough, 1);

    glUseProgram(0);
}

bool stgOSD::render(vbaTex *src) {
    if (!is_init) {
        return false;
    }

    // PART 1: stats

    // PART 2: scroll

    // PART 3: screen
    shader.enableVertAttrArr(glsl_vars.position);
    shader.enableVertAttrArr(glsl_vars.texcoord);
    renderStage::render(src);
    shader.disableVertAttrArr(glsl_vars.position);
    shader.disableVertAttrArr(glsl_vars.texcoord);

    // PART 4: text to screen
    gchar test = 'P';
    renderGlyph(g_utf8_get_char(&test), vbaSize(0, 0));


    return true;
}

