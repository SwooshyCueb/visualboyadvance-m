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

    setMult(STAGE_MULT);

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
    shader.setSrcTexUnit(idx);
    shader.setNeedsFlip(false);
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
