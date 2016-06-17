#include "swooshboy.h"
#include "tex.h"

vbaTex::vbaTex(uint mult, vbaGL *globj) {
    ctx = globj;
    scale = mult;
    if (mult) {
        size.x = ctx->base_sz.x * scale;
        size.y = ctx->base_sz.y * scale;
    } else {
        size = ctx->vwpt_sz;
    }
    unit = ctx->textures.size();
    glGenTextures(1, &texture);
    setData(NULL);
    setResizeFilter(GL_NEAREST);
    setOobBehavior(GL_CLAMP_TO_EDGE);
    hasBuffer = false;
    hasShader = false;
    if (mult) {
        ctx->largest_scale =
                (scale > ctx->largest_scale) ? scale : ctx->largest_scale;
    }
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

vbaTex::~vbaTex() {
    if (hasBuffer)
        remBuffer();
    glDeleteTextures(1, &texture);
}

inline void vbaTex::pushErr(vbaErrVal val, const char *file, int line,
                            const char *func) {
    return ctx->pushErr(val, file, line, func);
}
inline bool vbaTex::pushErrGL(const char *file, int line, const char *func) {
    return ctx->pushErrGL(file, line, func);
}
inline bool vbaTex::pushErrGL(vbaErrVal val, const char *file, int line,
                              const char *func) {
    return ctx->pushErrGL(val, file, line, func);
}
inline bool vbaTex::catchErrGL(GLenum ignore, const char *file, int line,
                               const char *func) {
    return ctx->catchErrGL(ignore, file, line, func);
}
inline bool vbaTex::catchErrGL(GLenum ignore, vbaErrVal val, const char *file,
                               int line, const char *func) {
    return ctx->catchErrGL(ignore, val, file, line, func);
}

bool vbaTex::initBuffer() {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture, 0);
    glDrawBuffers(1, ctx->DrawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    hasBuffer = true;
    return !errGLCheck();
}

bool vbaTex::remBuffer() {
    if (!hasBuffer)
        return false;
    glDeleteFramebuffers(1, &fbo);
    hasBuffer = false;
    return !errGLCheck();
}

bool vbaTex::bind() {
    //glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    //bind(0);
    return !errGLCheck();
}

bool vbaTex::bind(uint num) {
    glActiveTexture(GL_TEXTURE0 + num);
    glBindTexture(GL_TEXTURE_2D, texture);
    return !errGLCheck();
}

bool vbaTex::bindBuffer(GLenum target) {
    if (hasBuffer) {
        glBindFramebuffer(target, fbo);
        return !errGLCheck();
    } else {
        return false;
    }
}

bool vbaTex::bindBuffer() {
    return bindBuffer(GL_FRAMEBUFFER);
}

bool vbaTex::bindBufferRead() {
    return bindBuffer(GL_READ_FRAMEBUFFER);
}

bool vbaTex::bindBufferWrite() {
    return bindBuffer(GL_DRAW_FRAMEBUFFER);
}

bool vbaTex::setData(const GLvoid *data) {
    bind();
    glPixelStorei(GL_UNPACK_ROW_LENGTH, size.x + 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    return !errGLCheck();
}

void vbaTex::updSize() {
    if (scale) {
        size.x = ctx->base_sz.x * scale;
        size.y = ctx->base_sz.y * scale;
    } else {
        size = ctx->vwpt_sz;
    }
}

void vbaTex::setResizeFilter(GLint filter) {
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
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, behavior);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, behavior);
    errGLCheck();
}

void vbaTex::setShaderProg(glslProg *program) {
    prog = program;
    hasShader = true;
}
