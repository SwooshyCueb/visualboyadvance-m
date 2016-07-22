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

    setMult(STAGE_MULT);

    CREATE_GLSL_SRC_OBJ(passthru_glsl, passthrough);

    glslShader shd_p_f(globj, GL_FRAGMENT_SHADER);
    glslShader shd_p_v(globj, GL_VERTEX_SHADER);
    shd_p_f.setSrc(&passthru_glsl);
    shd_p_f.compile();
    shd_p_v.setSrc(&passthru_glsl);
    shd_p_v.compile();

    shader.init(ctx);

    shader.attachShader(&shd_p_f);
    shader.attachShader(&shd_p_v);

    shader.link();

    p_glsl_vars.position = shader.getAttrPtr("v_pos");
    p_glsl_vars.texcoord = shader.getAttrPtr("v_texcoord");
    p_glsl_vars.src_tex = shader.getUniformPtr("src_tex");
    p_glsl_vars.needs_flip = shader.getUniformPtr("needs_flip");

    shader.activate();

    shader.setVar1i(p_glsl_vars.needs_flip, 0);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
    shader.setVtxAttrPtr(p_glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
    shader.setVtxAttrPtr(p_glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CREATE_GLSL_SRC_OBJ(glyph_glsl, coloralph);

    glslShader shd_g_f(globj, GL_FRAGMENT_SHADER);
    glslShader shd_g_v(globj, GL_VERTEX_SHADER);
    shd_g_f.setSrc(&glyph_glsl);
    shd_g_f.compile();
    shd_g_v.setSrc(&glyph_glsl);
    shd_g_v.compile();

    shd_glyph.init(ctx);

    shd_glyph.attachShader(&shd_g_f);
    shd_glyph.attachShader(&shd_g_v);

    shd_glyph.link();

    g_glsl_vars.position = shd_glyph.getAttrPtr("v_pos");
    g_glsl_vars.texcoord = shd_glyph.getAttrPtr("v_texcoord");
    g_glsl_vars.src_tex = shd_glyph.getUniformPtr("src_tex");
    g_glsl_vars.needs_flip = shd_glyph.getUniformPtr("needs_flip");
    g_glsl_vars.fg_color = shd_glyph.getUniformPtr("fg_color");
    //g_glsl_vars.bg_color = shd_glyph.getUniformPtr("bg_color");

    shd_glyph.activate();

    shd_glyph.setVar1i(g_glsl_vars.needs_flip, 0);
    shd_glyph.setVar1i(g_glsl_vars.src_tex, 0);
    shd_glyph.setVar4f(g_glsl_vars.fg_color, 1.0, 1.0, 1.0, 0.6);
    //shd_glyph.setVar4f(g_glsl_vars.bg_color, 0.2, 0.2, 0.2, 0.6);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
    shd_glyph.setVtxAttrPtr(g_glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

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

    glGenBuffers(1, &vb_vtx);
    glGenBuffers(1, &vb_texcoord);


    is_init = true;

}

stgOSD::~stgOSD() {
    if (is_init) {
        is_init = false;
    }
}

bool stgOSD::pushText(gunichar *text) {
    // Create texture for new line of text
    osdLine *newline = new osdLine;
    uint idx = 0;
    float w = 4, h = FONT_SIZE + 4;

    for (gunichar c = text[idx]; c != '\0'; index++) {
        ftGlyph *g = fnt.getGlyph(c, FONT_SIZE);
        w += g->adv.x;
        if (w < getSize().xu()) {
            break;
        }
    }

    newline->tex.init(ctx, w, h);
    newline->pos = vbaSize(0, 0);

    glGenFramebuffers(1, &newline->buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, newline->buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, newline->tex.texture, 0);
    glDrawBuffers(1, ctx->DrawBuffers);

    // Render text to texture
    glClearColor(0.2, 0.2, 0.2, 0.6);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    w = 2;
    idx = 0;
    for (gunichar c = text[idx]; c != '\0'; index++) {
        ftGlyph *g = fnt.getGlyph(c, FONT_SIZE);
        renderGlyph(c, vbaSize(w, 2));
        w += g->adv.x;
        if (w < getSize().xu()) {
            break;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Change existing textures' positions
    std::deque<osdLine *>::iterator iter = scroll.begin();
    while(iter != scroll.end()) {
        (*iter)->pos += vbaSize(h, 0);
        iter++;
    }

    // Remove any textures pushed below midpoint of screen
    std::deque<osdLine *>::reverse_iterator riter = scroll.rbegin();
    while(riter != scroll.rend()) {
        if((*riter)->pos.xd() > (getSize().xd() * (gdouble)0.6)) {
            glDeleteFramebuffers(1, &((*riter)->buffer));
            scroll.pop_back();
        }
        riter++;
    }

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
    shader.setVar1i(p_glsl_vars.src_tex, (GLint)idx);
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
    shd_glyph.activate();

    glBindBuffer(GL_ARRAY_BUFFER, vb_vtx);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tpos, GL_DYNAMIC_DRAW);
    shd_glyph.setVtxAttrPtr(g_glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnable(GL_BLEND);

    shd_glyph.enableVertAttrArr(g_glsl_vars.position);
    shd_glyph.enableVertAttrArr(g_glsl_vars.texcoord);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    shd_glyph.disableVertAttrArr(g_glsl_vars.position);
    shd_glyph.disableVertAttrArr(g_glsl_vars.texcoord);

    glDisable(GL_BLEND);

    // We shouldn't have to do this?
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
    shd_glyph.setVtxAttrPtr(g_glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(0);
}

bool stgOSD::render(vbaTex *src) {
    if (!is_init) {
        return false;
    }

    // PART 1: stats

    // PART 2: scroll

    // PART 3: screen
    shader.enableVertAttrArr(p_glsl_vars.position);
    shader.enableVertAttrArr(p_glsl_vars.texcoord);
    renderStage::render(src);
    shader.disableVertAttrArr(p_glsl_vars.position);
    shader.disableVertAttrArr(p_glsl_vars.texcoord);

    // PART 4: text to screen
    //gchar test = 'P';
    //renderGlyph(g_utf8_get_char(&test), vbaSize(0, 0));


    return true;
}

