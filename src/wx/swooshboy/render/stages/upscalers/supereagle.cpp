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

    renderStage::shader = new glslProg(ctx);

    renderStage::shader->attachShader(shd_f);
    renderStage::shader->attachShader(shd_v);

    renderStage::shader->link();

    renderStage::setMult(STAGE_MULT);

    renderStage::is_init = true;
    return true;
}

bool stgSuperEagle::setIndex(uint idx, renderPipeline *rdrpth) {
    if (!renderStage::is_init) {
        return false;
    }
    renderStage::setIndex(idx, rdrpth);
    renderStage::shader->setSrcTexUnit(idx);
    renderStage::shader->setNeedsFlip(false);
    renderStage::shader->setDstSz(getSize());
    if (idx) {
        renderStage::shader->setSrcSz(stageAt(idx-1)->getSize());
    } else {
        renderStage::shader->setSrcSz(ctx->getBaseSize() * ctx->getBaseScale());
    }

    renderStage::init_s = true;

    return true;
}

bool stgSuperEagle::render(vbaTex *src) {
    if (!renderStage::is_init) {
        return false;
    }
    renderStage::render(src);

    return true;
}
