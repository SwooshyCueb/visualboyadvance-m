#include "swooshboy.h"
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
    shd_draw.setNeedsFlip(true);
    shd_draw.setSrcTexUnit(0);

    is_init = true;

    log_fixme("Logging demo", "So I wrote a bunch of ugly code,");
    log_todo("Logging demo", "and this ugly code has some pretty results.");
    log_debug("Logging demo", "It handles logging in a way similar to glib,");
    log_info("Logging demo", "and it makes the loging output easy to read.");
    log_warning_debug("Logging demo", "The code used to be a lot uglier.");
    log_message("Logging demo", "It also makes debugging a bit easier.");
    log_warning("Logging demo", "It's not quite done yet,");
    log_critical("Logging demo", "but it's good enough for now.");
    log_error_pass("Logging demo", "Actually it's not but LOOK PRETTY TERMINAL OUTPUT");

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
    ctx->draw();
    glUseProgram(0);
    return true;
}

renderStage *renderPipeline::stageAt(uint idx) {
    return pipeline[idx];
}

EH_DEFINE(renderPipeline);
