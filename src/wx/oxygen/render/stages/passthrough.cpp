#include "oxygen.h"
#include "render/pipeline.h"

#include "passthrough.h"
#define STAGE_MULT 1

stgPassthrough::stgPassthrough(vbaGL *globj)
    : renderStage(globj) {
    init(globj);
}

bool stgPassthrough::init(vbaGL *globj) {
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
    glsl_vars.position = shader.getAttrPtr("v_pos");
    glsl_vars.texcoord = shader.getAttrPtr("v_texcoord");
    glsl_vars.src_tex = shader.getUniformPtr("src_tex");
    glsl_vars.needs_flip = shader.getUniformPtr("needs_flip");

    setMult(STAGE_MULT);

    shader.setVar1i(glsl_vars.needs_flip, 0);

    shader.activate();
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
    shader.setVtxAttrPtr(glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);
    shader.enableVertAttrArr(glsl_vars.position);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
    shader.setVtxAttrPtr(glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    shader.enableVertAttrArr(glsl_vars.texcoord);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    is_init = true;
    return true;
}

stgPassthrough::~stgPassthrough() {
    // Nothing to do here yet
}

bool stgPassthrough::setIndex(uint idx, renderPipeline *rdrpth) {
    if (!is_init) {
        return false;
    }
    renderStage::setIndex(idx, rdrpth);

    shader.setVar1i(glsl_vars.src_tex, (GLint)idx);

    has_shader = true;

    return true;
}

bool stgPassthrough::render(vbaTex *src) {
    if (!is_init) {
        return false;
    }
    renderStage::render(src);

    return true;
}
