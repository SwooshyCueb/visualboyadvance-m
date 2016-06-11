#include <GL/glxew.h>
#include <GL/glut.h>
#include "render.h"
#include <cstdio>
#include <cstddef>
#include <deque>
#include <queue>

#ifndef VBA_TRIANGLE_STRIP
GLfloat vbaGL::draw_vert[8] = {
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0,
};
GLfloat vbaGL::draw_coord[8] = {
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0,
};
#else
GLint vbaGL::draw_vert[12] = {
    0, 0, 0,
    1, 0, 0,
    0, 1, 0,
    1, 1, 0,
};
GLfloat vbaGL::draw_coord[8] = {
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
};
#endif

GLenum vbaGL::DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};

inline bool vbaGL::glPushErr(const char *file, int line, const char *func) {
    GLenum val;
    bool ret = false;
    //glErr *err;

    while ((val = glGetError()) != GL_NO_ERROR) {
        //err = new glErr(val, file, line - 1, func);
        //glErrs.push(err);
        glErrs.emplace(val, file, line - 1, func);
        ret = true;
    }
    return ret;
}

glErr vbaGL::glErrGet() {
    if (glErrs.size()) {
        glErr ret;
        ret = glErrs.front();
        glErrs.pop();
        return ret;
    } else {
        throw VBAERR_NODATA;
    }
}

void vbaGL::glErrPrint() {
    uint errs = glErrs.size();
    for (uint i = 0; i < errs; i++) {
        glErrs.front().print();
        glErrs.pop();
    }
}

vbaGL::vbaGL() {
    if (glewInit() != GLEW_OK)
        throw VBAERR_GLINIT;
    glDisable(GL_CULL_FACE);
    glCheckErr();
    glEnable(GL_TEXTURE_2D);
    glCheckErr();
    glMatrixMode(GL_PROJECTION);
    glCheckErr();
    glEnableClientState(GL_VERTEX_ARRAY);
    glCheckErr();
    glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
    glCheckErr();
    glLoadIdentity();
    glCheckErr();
    //glOrtho(0.0, 1.0, 1.0, 0.0, 0.0, 1.0);
    glOrtho(0.0, 1.0, 1.0, 0.0, 1.0, 0.0);
    glCheckErr();
    //glMatrixMode(GL_TEXTURE);
    //glLoadIdentity();
    //glScalef(1.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glCheckErr();

    #ifndef VBA_TRIANGLE_STRIP
    glVertexPointer(2, GL_FLOAT, 0, draw_vert);
    #else
    glVertexPointer(3, GL_INT, 0, draw_vert);
    #endif
    glCheckErr();
    glTexCoordPointer(2, GL_FLOAT, 0, draw_coord);
    glCheckErr();
}

void vbaGL::setBaseSize(uint x, uint y) {
    base_sz.x = x;
    base_sz.y = y;
}

void vbaGL::setVwptSize(uint x, uint y) {
    vwpt_sz.x = x;
    vwpt_sz.y = y;
}

bool vbaGL::draw() {
    #ifndef VBA_TRIANGLE_STRIP
    glDrawArrays(GL_QUADS, 0, 4);
    #else
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    #endif
    glCheckErr();

    return true;
}

/* Might do this differently */
bool vbaGL::genTextures(uint scale) {
    textures.emplace_back(scale, this);
    return true;
}

/* Might do this differently */
bool vbaGL::setTexData(const GLvoid *data) {
    return textures[0].setData(data);
}

inline bool vbaTex::glPushErr(const char *file, int line, const char *func) {
    ctx->glPushErr(file, line, func);
}

vbaTex::vbaTex(uint mult, vbaGL *globj) {
    ctx = globj;
    scale = mult;
    unit = ctx->textures.size();
    glGenTextures(1, &texture);
    setData(NULL);
    setResizeFilter(GL_NEAREST);
    setOobBehavior(GL_CLAMP_TO_EDGE);
    hasBuffer = false;
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

bool vbaTex::initBuffer() {
    glGenFramebuffers(1, &texbuff);
    glGenRenderbuffers(1, &rdrbuff);
    bindBuffer();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, rdrbuff);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture, 0);
    glDrawBuffers(1, ctx->DrawBuffers);
    hasBuffer = true;
}

bool vbaTex::remBuffer() {
    if (!hasBuffer)
        return false;
    glDeleteFramebuffers(1, &texbuff);
    glDeleteRenderbuffers(1, &rdrbuff);
    hasBuffer = false;
    return true;
}

bool vbaTex::bind() {
    //glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    //bind(0);
    glCheckErr();
    return true;
}

bool vbaTex::bind(uint num) {
    glActiveTexture(GL_TEXTURE0 + num);
    glCheckErr();
    glBindTexture(GL_TEXTURE_2D, texture);
    glCheckErr();
    return true;
}

bool vbaTex::bindBuffer() {
    if (hasBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, texbuff);
        return true;
    } else {
        return false;
    }
}

bool vbaTex::bindBufferRead() {
    if (hasBuffer) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, texbuff);
        return true;
    } else {
        return false;
    }
}

bool vbaTex::bindBufferWrite() {
    if (hasBuffer) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, texbuff);
        return true;
    } else {
        return false;
    }
}

bool vbaTex::setData(const GLvoid *data) {
    vbaSize sz = getSize();
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sz.x, sz.y, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glCheckErr();
    return true;
}

vbaSize vbaTex::getSize(){
    vbaSize ret;
    if (!scale) {
        // Might use largest_scale here instead
        ret.x = ctx->vwpt_sz.x;
        ret.y = ctx->vwpt_sz.y;
    } else {
        ret.x = ctx->base_sz.x * scale;
        ret.y = ctx->base_sz.y * scale;
    }
    return ret;
}

void vbaTex::setResizeFilter(GLint filter) {
    resizefilt = filter;
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glCheckErr();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glCheckErr();
    if (filter = GL_NEAREST)
        blitmask = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    else
        blitmask = GL_COLOR_BUFFER_BIT;
    glCheckErr();
}

void vbaTex::setOobBehavior(GLint behavior) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, behavior);
    glCheckErr();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, behavior);
    glCheckErr();
}
