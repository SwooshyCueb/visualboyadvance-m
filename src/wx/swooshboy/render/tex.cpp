#include "swooshboy.h"
#include "tex.h"

vbaTex::vbaTex() {
    is_init = false;
}

vbaTex::vbaTex(float scale, vbaGL *globj) {
    init(scale, globj);
}

bool vbaTex::init(float scale, vbaGL *globj) {
    ctx = globj;
    if (scale) {
        size.x = ctx->base_sz.x * scale;
        size.y = ctx->base_sz.y * scale;
    } else {
        size = ctx->vwpt_sz;
    }
    glGenTextures(1, &texture);
    setData(NULL);
    setResizeFilter(GL_NEAREST);
    setOobBehavior(GL_CLAMP_TO_EDGE);
    if (scale) {
        ctx->largest_scale =
                (scale > ctx->largest_scale) ? scale : ctx->largest_scale;
    }
    glClearColor(0.0, 0.0, 0.0, 1.0);

    return true;
}

vbaTex::~vbaTex() {
    if (is_init) {
        glDeleteTextures(1, &texture);
        is_init = false;
    }
}

EH_DEFINE(vbaTex);
#if 0
bool vbaTex::remBuffer() {
    if (!hasBuffer)
        return false;
    glDeleteFramebuffers(1, &fbo);
    hasBuffer = false;
    return !errGLCheck();
}
#endif

bool vbaTex::bind() {
    if (is_init) {
        return false;
    }
    //glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    //bind(0);
    return !errGLCheck();
}

bool vbaTex::bind(uint num) {
    if (is_init) {
        return false;
    }
    glActiveTexture(GL_TEXTURE0 + num);
    glBindTexture(GL_TEXTURE_2D, texture);
    return !errGLCheck();
}

bool vbaTex::setData(const GLvoid *data) {
    if (is_init) {
        return false;
    }
    bind();
    glPixelStorei(GL_UNPACK_ROW_LENGTH, size.x + 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    return !errGLCheck();
}

void vbaTex::updSize(float scale) {
    if (is_init) {
        return;
    }
    if (scale) {
        size.x = ctx->base_sz.x * scale;
        size.y = ctx->base_sz.y * scale;
    } else {
        size = ctx->vwpt_sz;
    }
}

void vbaTex::setResizeFilter(GLint filter) {
    if (is_init) {
        return;
    }
    resizefilt = filter;
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    if (filter == GL_NEAREST)
        blitmask = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    else
        blitmask = GL_COLOR_BUFFER_BIT;
    errGLCheck();
}

void vbaTex::setOobBehavior(GLint behavior) {
    if (is_init) {
        return;
    }
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, behavior);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, behavior);
    errGLCheck();
}

vbaSize vbaTex::getSize() {
    vbaSize ret(size);
    return ret;
}
