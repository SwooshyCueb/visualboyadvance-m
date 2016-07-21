// Not yet functional

#include "oxygen.h"
#include "osd.h"

#include <glib.h>

#include "oxygen.h"
#include "render/text/text.h"

#include <vector>

#define STAGE_MULT 1

stgOSD::stgOSD(vbaGL *globj)
    : renderStage(globj) {
    init(globj);
}

bool stgOSD::init(vbaGL *globj) {
    //DECLARE_RES(ttf_neoletters);
    //DECLARE_RES(ttf_o2);

    CREATE_TTF_RES_OBJ(res_neoletters, neoletters);
    fnt.init(&ft, res_neoletters);
    fnt.cacheGlyphs(16);

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


    //glGenBuffers(1, &vb_vtx);
    //glGenBuffers(1, &vb_texcoord);


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

    return true;
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


    gchar test = 'P';
    ftGlyph *tglyph = fnt.getGlyph(g_utf8_get_char(&test), 16.0);
    tex_glyph.setSize(tglyph->sz_tex);
    tex_glyph.setData((GLvoid *)tglyph->data);

    gfloat sx = (gfloat)(((gdouble)1.0 / getSize().xd()) * tglyph->sz_tex.xd());
    gfloat sy = (gfloat)(((gdouble)1.0 / getSize().yd()) * tglyph->sz_tex.yd());

    GLfloat box[4][2] = {
        {0.5,    0.5},
        {0.5+sx, 0.5},
        {0.5,    0.5+sy},
        {0.5+sx, 0.5+sy}
    };
    log_info("Glyph Rendering Test",
             "sx: %g\n"
             "sy: %g",
             sx, sy);

    shader.activate();
    //shader.setVar1i(glsl_vars.is_passthrough, 0);
    tex_glyph.bind(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), box, GL_DYNAMIC_DRAW);
    shader.setVtxAttrPtr(glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    //shader.setVtxAttrPtr(glsl_vars.texcoord, 4, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(vbo);

    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnable(GL_BLEND);
    shader.enableVertAttrArr(glsl_vars.position);
    shader.enableVertAttrArr(glsl_vars.texcoord);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    shader.disableVertAttrArr(glsl_vars.position);
    shader.disableVertAttrArr(glsl_vars.texcoord);
    glDisable(GL_BLEND);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
    shader.setVtxAttrPtr(glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
    shader.setVtxAttrPtr(glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shader.setVar1i(glsl_vars.src_tex, index);
    shader.setVar1i(glsl_vars.is_passthrough, 1);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // PART 4: text to screen


    /*

    shader.activate();
    shader.setVar1i(glsl_vars.is_passthrough, 1);
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
    shader.setVtxAttrPtr(glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
    shader.setVtxAttrPtr(glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    */

    /*
    renderStage::render(src);
    tex_atlas.bind(index);

    //shader.disableVertAttrArr(glsl_vars.position);
    //shader.disableVertAttrArr(glsl_vars.texcoord);

    //glClearColor(255,255,255,0);
    //glClear(GL_COLOR_BUFFER_BIT);


    shader.activate();
    shader.setVar1i(glsl_vars.is_passthrough, 0);
    glEnable(GL_BLEND);

    char text[] = "This is a test, woo!\0";
    float tx, ty;
    tx = ty = 0.25;

    std::vector<gfloat> vpos;
    std::vector<gfloat> tcoord;

    char *cptr = text;
    uint charc = 0;

    while (*cptr) {
        stbtt_aligned_quad q;
        stbtt_GetPackedQuad(chardata, (NUM_GLYPHS * ATLAS_GLYPH_S),
                            (NUM_GLYPHS * ATLAS_GLYPH_S), *cptr++, &tx, &ty, &q,
                            0);
        tcoord.push_back(q.s0); tcoord.push_back(q.t0);
        tcoord.push_back(q.s1); tcoord.push_back(q.t0);
        tcoord.push_back(q.s0); tcoord.push_back(q.t1);
        tcoord.push_back(q.s1); tcoord.push_back(q.t1);

        vpos.push_back(q.x0);vpos.push_back(q.y0);
        vpos.push_back(q.x1);vpos.push_back(q.y0);
        vpos.push_back(q.x0);vpos.push_back(q.y1);
        vpos.push_back(q.x1);vpos.push_back(q.y1);

        charc++;
    }


    glBindBuffer(GL_ARRAY_BUFFER, vb_vtx);
    glBufferData(GL_ARRAY_BUFFER, charc * sizeof(GLfloat), vpos.data(), GL_DYNAMIC_DRAW);
    shader.setVtxAttrPtr(glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(vb_vtx);

    glBindBuffer(GL_ARRAY_BUFFER, vb_texcoord);
    glBufferData(GL_ARRAY_BUFFER, charc * sizeof(GLfloat), tcoord.data(), GL_DYNAMIC_DRAW);
    shader.setVtxAttrPtr(glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(vb_texcoord);

    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for(int i = 0; i < charc; i++) {
        glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
    }
    //glDrawArrays(GL_LINE_STRIP, 0, charc * 4);

    glDrawArrays(GL_TRIANGLE_FAN, 0,

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
    shader.setVtxAttrPtr(glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
    shader.setVtxAttrPtr(glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    */
    /*

    glBindFramebuffer(GL_FRAMEBUFFER, buffer);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glUseProgram(0);
    */
    //shader.disableVertAttrArr(glsl_vars.position);
    //shader.disableVertAttrArr(glsl_vars.texcoord);
    //glDisableVertexAttribArray(vb_vtx);
    //glDisableVertexAttribArray(vb_texcoord);

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glDisable(GL_BLEND);

    return true;
}

