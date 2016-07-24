#include "oxygen.h"
#include "pipeline.h"
#include "vbaerr.h"

renderPipeline::renderPipeline() {
    is_init = false;
}

renderPipeline::renderPipeline(vbaGL *globj) {
    init(globj);
}

bool renderPipeline::init(vbaGL *globj) {
    ctx = globj;

    // Initialize base texture
    base.init(ctx, ctx->getBaseSize() * ctx->getBaseScale());

    // Setup drawing shader
    CREATE_GLSL_SRC_OBJ(draw_src, passthrough);
    glslShader draw_f(ctx, GL_FRAGMENT_SHADER);
    glslShader draw_v(ctx, GL_VERTEX_SHADER);
    draw_f.setSrc(&draw_src);
    draw_f.compile();
    draw_v.setSrc(&draw_src);
    draw_v.compile();
    shd_draw.init(ctx);
    shd_draw.attachShader(&draw_f);
    shd_draw.attachShader(&draw_v);
    shd_draw.link();

    glsl_vars.position = shd_draw.getAttrPtr("v_pos");
    glsl_vars.texcoord = shd_draw.getAttrPtr("v_texcoord");
    glsl_vars.src_tex = shd_draw.getUniformPtr("src_tex");
    glsl_vars.needs_flip = shd_draw.getUniformPtr("needs_flip");

    shd_draw.setVar1i(glsl_vars.needs_flip, 1);
    shd_draw.setVar1i(glsl_vars.src_tex, 0);

    shd_draw.activate();
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
    shd_draw.setVtxAttrPtr(glsl_vars.position, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
    shd_draw.setVtxAttrPtr(glsl_vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    is_init = true;

    return true;
}

renderPipeline::~renderPipeline() {
    //when pipeline stores objects instead of pointers, uncomment this
    //pipeline.clear();
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
    base.setData((const GLvoid *)data);

    std::deque<renderStage *>::iterator  iter = pipeline.begin();
    prev = &base;

    while(iter != pipeline.end()) {
        (*iter)->render(prev);
        prev = &((*iter)->texture);
        iter++;
    }

    return true;
}

bool renderPipeline::draw() {
    if (!is_init) {
        return false;
    }
    pipeline.back()->texture.bind(0);
    shd_draw.activate();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ctx->glVwpt(ctx->getVwptSize());

    shd_draw.enableVertAttrArr(glsl_vars.position);
    shd_draw.enableVertAttrArr(glsl_vars.texcoord);
    ctx->draw();
    shd_draw.disableVertAttrArr(glsl_vars.position);
    shd_draw.disableVertAttrArr(glsl_vars.texcoord);

    glUseProgram(0);
    return true;
}

renderStage *renderPipeline::stageAt(uint idx) {
    return pipeline[idx];
}

EH_DEFINE(renderPipeline);
