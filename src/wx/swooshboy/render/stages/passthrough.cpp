#include "swooshboy.h"
#include "render/pipeline.h"

#include "passthrough.h"
#define STAGE_MULT 1

stgPassthrough::stgPassthrough(vbaGL *globj)
    : renderStage(globj) {

    CREATE_GLSL_SRC_OBJ(pt_src, passthrough);

    glslShader shd_f(globj, GL_FRAGMENT_SHADER);
    glslShader shd_v(globj, GL_VERTEX_SHADER);
    shd_f.setSrc(&pt_src);
    shd_f.compile();
    shd_v.setSrc(&pt_src);
    shd_v.compile();

    renderStage::shader = new glslProg(ctx);

    renderStage::shader->attachShader(shd_f);
    renderStage::shader->attachShader(shd_v);

    renderStage::shader->init();

    renderStage::setMult(STAGE_MULT);

}

bool stgPassthrough::setIndex(uint idx) {
    renderStage::setIndex(idx);
    renderStage::shader->setSrcTexUnit(idx);
    renderStage::shader->setNeedsFlip(false);
    renderStage::init_s = true;

    return true;
}

bool stgPassthrough::render(vbaTex *src) {
    renderStage::render(src);

    return true;
}
