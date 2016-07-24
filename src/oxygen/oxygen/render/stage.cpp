#include "oxygen.h"
#include "stage.h"

renderStage::renderStage() {
    is_init = false;
}

renderStage::renderStage(vbaGL *globj) {
    init(globj);
}

bool renderStage::init(vbaGL *globj) {
    ctx = globj;
    is_init = true;
    return true;
}

renderStage::~renderStage() {
    if (has_buffer) {
        glDeleteFramebuffers(1, &buffer);
    }
}

vbaSize renderStage::getSize() {
    vbaSize ret(texture.getSize());
    return ret;
}

renderStage *renderStage::stageAt(uint idx) {
    return pipeline->stageAt(idx);
}

bool renderStage::setMult(uint coeff) {
    if (!is_init) {
        return false;
    }
    mult = coeff;
    mult_set = true;

    return true;
}

bool renderStage::setIndex(uint idx, renderPipeline *rdrpth) {
    if (!is_init) {
        return false;
    }
    if (!mult_set) {
        return false;
    }

    pipeline = rdrpth;

    if (idx) {
        scale = stageAt(idx-1)->scale * mult;
    } else {
        scale = ctx->getBaseScale() * mult;
    }

    if (!has_texture) {
        if (scale)
            texture.init(ctx, scale * ctx->getBaseSize());
        else
            texture.init(ctx, ctx->getVwptSize());
        has_texture = true;
    } else {
        if (scale)
            texture.setSize(scale * ctx->getBaseSize());
        else
            texture.setSize(ctx->getVwptSize());
    }
    if (!has_buffer) {
        glGenFramebuffers(1, &buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, buffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, texture.texture, 0);
        glDrawBuffers(1, ctx->DrawBuffers);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        has_buffer = true;
    }
    index = idx;
    return true;
}

bool renderStage::render(vbaTex *src) {
    src->bind(index);
    shader.activate();
    glPixelStorei(GL_UNPACK_ROW_LENGTH, src->size.xu() + 1);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    ctx->glVwpt(texture.size);
    ctx->draw();
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

EH_DEFINE(renderStage);
