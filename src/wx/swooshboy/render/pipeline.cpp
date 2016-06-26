#include "swooshboy.h"
#include "pipeline.h"
#include "vbaerr.h"

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

vbaSize renderStage::getSize() {
    vbaSize ret(texture->getSize());
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
    init_m = true;

    return true;
}

bool renderStage::setIndex(uint idx, renderPipeline *rdrpth) {
    if (!is_init) {
        return false;
    }
    if (!init_m) {
        return false;
    }

    pipeline = rdrpth;

    if (idx) {
        scale = stageAt(idx-1)->scale * mult;
    } else {
        scale = ctx->getBaseScale() * mult;
    }

    if (!init_t) {
        if (scale)
            texture = new vbaTex(ctx, scale * ctx->getBaseSize());
        else
            texture = new vbaTex(ctx, ctx->getVwptSize());
        init_t = true;
    } else {
        if (scale)
            texture->setSize(scale * ctx->getBaseSize());
        else
            texture->setSize(ctx->getVwptSize());
    }
    if (!init_b) {
        glGenFramebuffers(1, &buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, buffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, texture->texture, 0);
        glDrawBuffers(1, ctx->DrawBuffers);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        init_b = true;
    }
    index = idx;
    return true;
}

bool renderStage::render(vbaTex *src) {
    src->bind(index);
    shader->activate();
    glPixelStorei(GL_UNPACK_ROW_LENGTH, src->size.x + 1);
    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    ctx->glVwpt(texture->size);
    ctx->draw();
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

EH_DEFINE(renderStage);

renderPipeline::renderPipeline() {
    is_init = false;
}

renderPipeline::renderPipeline(vbaGL *globj) {
    init(globj);
}

bool renderPipeline::init(vbaGL *globj) {
    ctx = globj;

    // Initialize base texture
    base = new vbaTex(ctx, ctx->getBaseSize() * ctx->getBaseScale());

    // Setup drawing shader
    CREATE_GLSL_SRC_OBJ(draw_src, passthrough);
    glslShader draw_f(ctx, GL_FRAGMENT_SHADER);
    glslShader draw_v(ctx, GL_VERTEX_SHADER);
    draw_f.setSrc(&draw_src);
    draw_f.compile();
    draw_v.setSrc(&draw_src);
    draw_v.compile();
    shd_draw = new glslProg(ctx);
    shd_draw->attachShader(draw_f);
    shd_draw->attachShader(draw_v);
    shd_draw->link();
    shd_draw->setNeedsFlip(true);
    shd_draw->setSrcTexUnit(0);

    is_init = true;

    return true;
}

bool renderPipeline::addStage(renderStage *stg) {
    if (!is_init) {
        return false;
    }
    stg->setIndex(pipeline.size(), this);
    pipeline.push_back(stg);
    return true;
}

bool renderPipeline::removeStage(uint idx) {
    if (!is_init) {
        return false;
    }
    pipeline.erase(pipeline.begin() + idx);

    if (idx || (pipeline.begin() + (idx - 1) == pipeline.end())) {
        return true;
    }

    std::deque<renderStage *>::iterator  iter = pipeline.begin() + idx;

    while(iter != pipeline.end()) {
        (*iter)->setIndex((*iter)->index - 1, this);
        iter++;
    }
    return true;
}

bool renderPipeline::refreshStages() {
    if (!is_init) {
        return false;
    }
    if (pipeline.empty())
        return true;
    std::deque<renderStage *>::iterator  iter = pipeline.begin();

    while(iter != pipeline.end()) {
        (*iter)->setIndex((*iter)->index, this);
        iter++;
    }
    return true;
}

uint renderPipeline::getSize() {
    if (!is_init) {
        return 0;
    }
    return pipeline.size();
}

bool renderPipeline::render(const void *data) {
    if (!is_init) {
        return false;
    }
    vbaTex *prev;
    base->setData((const GLvoid *)data);

    std::deque<renderStage *>::iterator  iter = pipeline.begin();
    prev = base;

    while(iter != pipeline.end()) {
        (*iter)->render(prev);
        prev = (*iter)->texture;
        iter++;
    }

    return true;
}

bool renderPipeline::draw() {
    if (!is_init) {
        return false;
    }
    pipeline.back()->texture->bind(0);
    shd_draw->activate();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ctx->glVwpt(ctx->getVwptSize());
    ctx->draw();
    glUseProgram(0);
    return true;
}

renderStage *renderPipeline::stageAt(uint idx) {
    return pipeline[idx];
}

EH_DEFINE(renderPipeline);
