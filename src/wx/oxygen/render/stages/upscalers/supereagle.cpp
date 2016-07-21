#include "oxygen.h"
#include "render/stage.h"

#include "supereagle.h"
#define STAGE_MULT 2

stgSuperEagle::stgSuperEagle(vbaGL *globj)
    : renderStage(globj) {
    init(globj);
}

bool stgSuperEagle::init(vbaGL *globj) {
    CREATE_GLSL_SRC_OBJ(se_src, supereagle);

    glslShader shd_f(globj, GL_FRAGMENT_SHADER);
    glslShader shd_v(globj, GL_VERTEX_SHADER);
    shd_f.setSrc(&se_src);
    shd_f.compile();
    shd_v.setSrc(&se_src);
    shd_v.compile();

    shader.init(ctx);

    shader.attachShader(&shd_f);
    shader.attachShader(&shd_v);

    shader.link();
    glsl_vars.position = shader.getAttrPtr("v_pos");
    glsl_vars.texcoord = shader.getAttrPtr("v_texcoord");
    glsl_vars.f_dst_sz = shader.getUniformPtr("f_dst_sz");
    glsl_vars.f_src_sz = shader.getUniformPtr("f_src_sz");
    glsl_vars.v_dst_sz = shader.getUniformPtr("v_dst_sz");
    glsl_vars.v_src_sz = shader.getUniformPtr("v_src_sz");
    glsl_vars.src_tex = shader.getUniformPtr("src_tex");
    glsl_vars.needs_flip = shader.getUniformPtr("needs_flip");

    setMult(STAGE_MULT);

    shader.setVar1i(glsl_vars.needs_flip, 0);

    shader.activate();
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
    shader.setVtxAttrPtr(glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
    shader.setVtxAttrPtr(glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    setMult(STAGE_MULT);

    is_init = true;
    return true;
}

stgSuperEagle::~stgSuperEagle() {
    // Nothing to do here yet
}

bool stgSuperEagle::setIndex(uint idx, renderPipeline *rdrpth) {
    if (!is_init) {
        return false;
    }
    renderStage::setIndex(idx, rdrpth);

    vbaSize sz = getSize();

    shader.setVar1i(glsl_vars.src_tex, (GLint)idx);

    if (idx) {
        sz = stageAt(idx-1)->getSize();
    } else {
        sz = (ctx->getBaseSize() * ctx->getBaseScale());
    }

    shader.setVar2f(glsl_vars.v_src_sz, sz.xf(), sz.yf());
    shader.setVar2f(glsl_vars.f_src_sz, sz.xf(), sz.yf());

    has_shader = true;

    return true;
}

bool stgSuperEagle::render(vbaTex *src) {
    if (!is_init) {
        return false;
    }

    shader.enableVertAttrArr(glsl_vars.position);
    shader.enableVertAttrArr(glsl_vars.texcoord);
    renderStage::render(src);
    shader.disableVertAttrArr(glsl_vars.position);
    shader.disableVertAttrArr(glsl_vars.texcoord);

    return true;
}
