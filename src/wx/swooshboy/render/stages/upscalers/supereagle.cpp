#include "swooshboy.h"
#include "render/pipeline.h"

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

    shader.attachShader(shd_f);
    shader.attachShader(shd_v);

    shader.link();

    setMult(STAGE_MULT);

    is_init = true;
    return true;
}

bool stgSuperEagle::setIndex(uint idx, renderPipeline *rdrpth) {
    if (!is_init) {
        return false;
    }
    renderStage::setIndex(idx, rdrpth);
    shader.setSrcTexUnit(idx);
    shader.setNeedsFlip(false);
    shader.setDstSz(getSize());
    if (idx) {
        shader.setSrcSz(stageAt(idx-1)->getSize());
    } else {
        shader.setSrcSz(ctx->getBaseSize() * ctx->getBaseScale());
    }

    init_s = true;

    return true;
}

bool stgSuperEagle::render(vbaTex *src) {
    if (!is_init) {
        return false;
    }
    renderStage::render(src);

    return true;
}
