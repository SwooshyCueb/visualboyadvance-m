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

    shd_glyph.activate();

    shd_glyph.setVar1i(g_glsl_vars.needs_flip, 0);
    shd_glyph.setVar1i(g_glsl_vars.src_tex, 0);
    shd_glyph.setVar4f(g_glsl_vars.fg_color, 1.0, 1.0, 1.0, 0.6);
    //shd_glyph.setVar4f(g_glsl_vars.bg_color, 0.2, 0.2, 0.2, 0.6);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
    shd_glyph.setVtxAttrPtr(g_glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CREATE_GLSL_SRC_OBJ(line_glsl, osd);

    glslShader shd_l_f(globj, GL_FRAGMENT_SHADER);
    glslShader shd_l_v(globj, GL_VERTEX_SHADER);
    shd_l_f.setSrc(&line_glsl);
    shd_l_f.compile();
    shd_l_v.setSrc(&line_glsl);
    shd_l_v.compile();

    shd_line.init(ctx);

    shd_line.attachShader(&shd_l_f);
    shd_line.attachShader(&shd_l_v);

    shd_line.link();

    l_glsl_vars.position = shd_line.getAttrPtr("v_pos");
    l_glsl_vars.texcoord = shd_line.getAttrPtr("v_texcoord");
    l_glsl_vars.src_tex = shd_line.getUniformPtr("src_tex");
    l_glsl_vars.needs_flip = shd_line.getUniformPtr("needs_flip");
    l_glsl_vars.fade = shd_line.getUniformPtr("fade");

    shd_line.activate();

    shd_line.setVar1i(l_glsl_vars.needs_flip, 0);
    shd_line.setVar1i(l_glsl_vars.src_tex, 0);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
    shd_glyph.setVtxAttrPtr(l_glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Size here doesn't matter
    tex_glyph.init(globj, 4, 4, GL_ALPHA);
    tex_glyph.bind(0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    tex_stats.tex.init(globj, 4, 4);

    glGenFramebuffers(1, &tex_stats.buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, tex_stats.buffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, tex_stats.tex.texture, 0);
    glDrawBuffers(1, ctx->DrawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //tex_atlas.init(globj, (NUM_GLYPHS * ATLAS_GLYPH_S),
    //               (NUM_GLYPHS * ATLAS_GLYPH_S), GL_ALPHA);
    //tex_atlas.setData((GLvoid *)atlaspx);

    glGenBuffers(1, &vb_vtx);


    is_init = true;

}

stgOSD::~stgOSD() {
    if (is_init) {
        is_init = false;
    }
}

bool stgOSD::setStatPtrs(guint *fps, guint *speed) {
    fps_ptr = fps;
    speed_ptr = speed;
    return true;
}

bool stgOSD::pushText(gunichar *text) {
    gdouble adv_pad = (gdouble)FONT_SIZE / (gdouble)12;
    gfloat line_pad = (gfloat)FONT_SIZE/ 6.0;
    gfloat line_pad_lower = (gfloat)FONT_SIZE/ 3.0;
    // Create texture for new line of text
    osdLine *newline = new osdLine;
    guint idx = 0;
    gfloat w = line_pad * 2.0, h = FONT_SIZE + line_pad + line_pad_lower;

    for (gunichar c = text[idx]; c != '\0'; c = text[++idx]) {
        ftGlyph *g = fnt.getGlyph(c, FONT_SIZE);
        w += (gfloat)(((gdouble)g->adv.x / (gdouble)64) + adv_pad);
        if (w > getSize().xu()) {
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
    glClearColor(0.2, 0.2, 0.2, 0.8);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    w = line_pad;
    idx = 0;
    for (gunichar c = text[idx]; c != '\0'; c = text[++idx]) {
        ftGlyph *g = fnt.getGlyph(c, FONT_SIZE);
        renderGlyph(c, vbaSize(w, line_pad));
        w += (gfloat)(((gdouble)g->adv.x / (gdouble)64) + adv_pad);
        if (w > getSize().xu()) {
            break;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Change existing textures' positions
    std::deque<osdLine *>::iterator iter = scroll.begin();
    while(iter != scroll.end()) {
        (*iter)->pos += vbaSize(0, h);
        iter++;
    }

    // Remove any textures pushed below midpoint of screen
    std::deque<osdLine *>::reverse_iterator riter = scroll.rbegin();
    while(riter != scroll.rend()) {
        if((*riter)->pos.yd() > (getSize().yd() * (gdouble)0.6)) {
            glDeleteFramebuffers(1, &((*riter)->buffer));
            osdLine *oldLine = *riter;
            delete oldLine;
            scroll.pop_back();
        }
        riter++;
    }

    // Add texture to queue
    scroll.push_front(newline);

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

    gfloat line_pad = (gfloat)FONT_SIZE/ 6.0;
    gfloat line_pad_lower = (gfloat)FONT_SIZE/ 3.0;

    tex_stats.tex.setSize(getSize().xf(),
                          (gfloat)FONT_SIZE + line_pad + line_pad_lower);
    tex_stats.tex.setData(NULL);
    tex_stats.pos = vbaSize(0.0, getSize().yf() - tex_stats.tex.getSize().yf());

    gchar* _test = "This is a test string.\0";
    gunichar* test = g_utf8_to_ucs4_fast(_test, -1, NULL);
    pushText(test);
    g_free(test);

    return true;
}

bool stgOSD::renderGlyph(gunichar character, vbaSize pos) {
    ftGlyph *g = fnt.getGlyph(character, FONT_SIZE);

    if (!(g->sz_tex.xu() && g->sz_tex.yu())) {
        return true;
    }

    // we need to bring down the number of glTexImage2D calls
    tex_glyph.setSize(g->sz_tex - vbaSize(1,0));
    tex_glyph.setData((GLvoid *)g->data);
    tex_glyph.bind(0);

    vbaSize ioff = vbaSize(g->sz_glyph.xd(),
                           (gdouble)FONT_SIZE - g->sz_glyph.yd());
    vbaSize ipos = pos + ioff;
    vbaSize iend = (ipos + (g->sz_tex));
    vbaSize fpos = (ipos * sz_texel) - 1.0;
    vbaSize fend = (iend * sz_texel) - 1.0;

    /*
    guint i, j, p, c;
    guint bmst = (g->sz_tex.xu() * 3)+4;
    guint bmsz = bmst * g->sz_tex.yu() + 2;
    gchar *bitmap = (gchar *)g_malloc0(sizeof(gchar)*bmsz);
    gchar *u8c = (gchar *)g_malloc0(sizeof(gchar)*7);
    g_unichar_to_utf8(character, u8c);
    if ((g->sz_tex.xu() * g->sz_tex.yu()) > 0) {
        for (i = 0; i < g->sz_tex.yu(); i++) {
            bitmap[i*bmst] = ' ';
            bitmap[(i*bmst)+1] = ' ';
            bitmap[(i*bmst)+2] = ' ';
            bitmap[(i*bmst)+3] = ' ';
            for(j = 0; j < g->sz_tex.xu(); j++) {
                p = (i*bmst+(j*3))+4;
                c = i*g->sz_tex.xu()+j;
                g_snprintf(&bitmap[p], 4, "%.2x ", (uint)g->data[c]);
            }
            bitmap[(i+1)*bmst - 1] = '\n';
        }
        bitmap[bmsz-3] = '\0';
    }

    log_debug("Rendering glyph",
              "%s glyph info:\n"
              "value: 0x%04ix\n"
              "sz_tex: %u * %u\n"
              "offset: %d * %d (%d * %d)\n"
              "adv: %u * %u (%g * %g)\n"
              "data:\n%s",
              u8c,
              (guint32)character,
              g->sz_tex.xu(), g->sz_tex.yu(),
              g->sz_glyph.xi(), g->sz_glyph.yi(),
              ioff.xi(), ioff.yi(),
              g->adv.x, g->adv.y,
              (gfloat)((gdouble)g->adv.x / (gdouble)64),
              (gfloat)((gdouble)g->adv.y / (gdouble)64),
              bitmap);
    g_free(bitmap);
    g_free(u8c);
    */

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

bool stgOSD::renderLine(osdLine *line, bool fade) {
    line->tex.bind(0);

    // We should probably move this to the push function
    vbaSize fpos = (line->pos * sz_texel) - 1.0;
    vbaSize fend = ((line->pos + line->tex.getSize()) * sz_texel) - 1.0;

    GLfloat tpos[4][2] = {
        {fpos.xf(), fpos.yf()},
        {fend.xf(), fpos.yf()},
        {fend.xf(), fend.yf()},
        {fpos.xf(), fend.yf()}
    };

    shd_line.activate();

    glBindBuffer(GL_ARRAY_BUFFER, vb_vtx);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tpos, GL_DYNAMIC_DRAW);
    shd_line.setVtxAttrPtr(l_glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    float fadeval;
    if (fade) {
        fadeval = line->timeout--;
    } else {
        fadeval = 255;
    }
    shd_line.setVar1f(l_glsl_vars.fade, (gfloat)fadeval);

    glEnable(GL_BLEND);

    shd_line.enableVertAttrArr(l_glsl_vars.position);
    shd_line.enableVertAttrArr(l_glsl_vars.texcoord);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    shd_line.disableVertAttrArr(l_glsl_vars.position);
    shd_line.disableVertAttrArr(l_glsl_vars.texcoord);

    glDisable(GL_BLEND);

    // We shouldn't have to do this?
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
    shd_line.setVtxAttrPtr(l_glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(0);
}

bool stgOSD::render(vbaTex *src) {
    if (!is_init) {
        return false;
    }

    // PART 1: stats
    gdouble adv_pad = (gdouble)FONT_SIZE / (gdouble)12;
    gfloat line_pad = (gfloat)FONT_SIZE/ 6.0;
    glBindFramebuffer(GL_FRAMEBUFFER, tex_stats.buffer);
    glClearColor(0.2, 0.2, 0.2, 0.8);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    gfloat w = line_pad;
    gchar *statstr = g_strdup_printf("FPS: %3u (%3u%%)", *fps_ptr, *speed_ptr);
    gunichar *text = g_utf8_to_ucs4_fast(statstr, -1, NULL);
    g_free(statstr);
    guint idx = 0;
    for (gunichar c = text[idx]; c != '\0'; c = text[++idx]) {
        ftGlyph *g = fnt.getGlyph(c, FONT_SIZE);
        renderGlyph(c, vbaSize(w, line_pad));
        w += (gfloat)(((gdouble)g->adv.x / (gdouble)64) + adv_pad);
    }
    g_free(text);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // PART 2: screen
    shader.enableVertAttrArr(p_glsl_vars.position);
    shader.enableVertAttrArr(p_glsl_vars.texcoord);
    renderStage::render(src);
    shader.disableVertAttrArr(p_glsl_vars.position);
    shader.disableVertAttrArr(p_glsl_vars.texcoord);

    // PART 3: scroll
    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    std::deque<osdLine *>::reverse_iterator riter = scroll.rbegin();
    while(riter != scroll.rend()) {
        renderLine(*riter, true);
        if((*riter)->timeout <= 0) {
            glDeleteFramebuffers(1, &((*riter)->buffer));
            scroll.pop_back();
        }
        riter++;
    }

    // PART 4: stats
    renderLine(&tex_stats, false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    return true;
}

