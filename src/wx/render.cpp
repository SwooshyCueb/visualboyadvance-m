#include <GL/glxew.h>
#include <GL/glut.h>
#include "render.h"
#include <cstdio>
#include <cstddef>
#include <deque>
#include <queue>
#include <stack>

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
        vbaErrs.push(VBAERR_GLERR);
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
    if (glewInit() != GLEW_OK) {
        vbaErrs.push(VBAERR_GLINIT);
        throw VBAERR_GLINIT;
    }
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 1.0, 0.0, 0.0, 1.0);
    //glOrtho(0.0, 1.0, 1.0, 0.0, 1.0, 0.0);
    //glMatrixMode(GL_TEXTURE);
    //glLoadIdentity();
    //glScalef(1.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);

    #ifndef VBA_TRIANGLE_STRIP
    glVertexPointer(2, GL_FLOAT, 0, draw_vert);
    #else
    glVertexPointer(3, GL_INT, 0, draw_vert);
    #endif
    glTexCoordPointer(2, GL_FLOAT, 0, draw_coord);
    if (glCheckErr()) {
        throw VBAERR_GLERR;
    }
}

void vbaGL::setBaseSize(uint x, uint y) {
    base_sz.x = x;
    base_sz.y = y;

    for (uint i = 0; i < textures.size(); i++) {
        textures[i].updSize();
    }
}

void vbaGL::setVwptSize(uint x, uint y) {
    vwpt_sz.x = x;
    vwpt_sz.y = y;
    glViewport(0, 0, x, y);

    for (uint i = 0; i < textures.size(); i++) {
        textures[i].updSize();
    }
}

vbaDrawArrs vbaGL::genDrawArrs(uint x, uint y) {
    vbaDrawArrs ret;

    #ifndef VBA_TRIANGLE_STRIP
    ret.vert[0] = 0.0;      ret.vert[1] = 0.0;
    ret.vert[2] = float(x); ret.vert[3] = 0.0;
    ret.vert[4] = float(x); ret.vert[5] = float(y);
    ret.vert[6] = 0.0;      ret.vert[7] = float(y);

    ret.coord[0] = 0.0;         ret.coord[1] = 0.0;
    ret.coord[2] = float(x);    ret.coord[3] = 0.0;
    ret.coord[4] = float(x);    ret.coord[5] = float(y);
    ret.coord[6] = 0.0;         ret.coord[7] = float(y);
    #else
    ret.vert[0]  = 0;       ret.vert[1]  = 0;       ret.vert[2]  = 0;
    ret.vert[3]  = int(x);  ret.vert[4]  = 0;       ret.vert[5]  = 0;
    ret.vert[6]  = 0;       ret.vert[7]  = int(y);  ret.vert[8]  = 0;
    ret.vert[9]  = int(x);  ret.vert[10] = int(y);  ret.vert[11] = 0;

    ret.coord[0] = 0.0;         ret.coord[1] = 0.0;
    ret.coord[2] = float(x);    ret.coord[3] = 0.0;
    ret.coord[4] = 0.0;         ret.coord[5] = float(y);
    ret.coord[6] = float(x);    ret.coord[7] = float(y);
    #endif
    return ret;
}

bool vbaGL::draw() {
    for (uint i = 0; i < textures.size(); i++) {
        textures[i].bind();
        glPixelStorei(GL_UNPACK_ROW_LENGTH, textures[i].size.x + 1);
        if (i != textures.size() - 1) {
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            /*
            vbaDrawArrs drawArrs = genDrawArrs(textures[i+1].size.x,
                                               textures[i+1].size.y);
            #ifndef VBA_TRIANGLE_STRIP
            glVertexPointer(2, GL_FLOAT, 0, drawArrs.vert);
            #else
            glVertexPointer(3, GL_INT, 0, drawArrs.vert);
            #endif
            glTexCoordPointer(2, GL_FLOAT, 0, drawArrs.coord);
            */
            textures[i+1].bindBuffer();
            //glViewport(0, 0, textures[i+1].size.x, textures[i+1].size.y);
            //glViewport(0, 0, 1, 1);
        } else {
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            /*
            #ifndef VBA_TRIANGLE_STRIP
            glVertexPointer(2, GL_FLOAT, 0, draw_vert);
            #else
            glVertexPointer(3, GL_INT, 0, draw_vert);
            #endif
            glTexCoordPointer(2, GL_FLOAT, 0, draw_coord);
            */
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glViewport(0, 0, vwpt_sz.x, vwpt_sz.y);
        }
        #ifndef VBA_TRIANGLE_STRIP
        glDrawArrays(GL_QUADS, 0, 4);
        #else
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        #endif
    }
    return !glCheckErr();
}

void vbaGL::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

/* Might do this differently */
bool vbaGL::genTextures(uint scale) {
    textures.emplace_back(scale, this);
    textures.back().initBuffer();
    textures.emplace_back(scale, this);
    return true;
}

/* Might do this differently */
bool vbaGL::setTexData(const GLvoid *data) {
    return textures[0].setData(data);
}

inline bool vbaTex::glPushErr(const char *file, int line, const char *func) {
    return ctx->glPushErr(file, line, func);
}

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
    glBindFramebuffer(GL_FRAMEBUFFER, texbuff);
    //glBindRenderbuffer(GL_RENDERBUFFER, rdrbuff);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    //                         GL_RENDERBUFFER, rdrbuff);
    glCheckErr();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture, 0);
    glCheckErr();
    glDrawBuffers(1, ctx->DrawBuffers);
    glCheckErr();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    hasBuffer = true;
    return !glCheckErr();
}

bool vbaTex::remBuffer() {
    if (!hasBuffer)
        return false;
    glDeleteFramebuffers(1, &texbuff);
    glDeleteRenderbuffers(1, &rdrbuff);
    hasBuffer = false;
    return !glCheckErr();
}

bool vbaTex::bind() {
    //glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    //bind(0);
    return !glCheckErr();
}

bool vbaTex::bind(uint num) {
    glActiveTexture(GL_TEXTURE0 + num);
    glBindTexture(GL_TEXTURE_2D, texture);
    return !glCheckErr();
}

bool vbaTex::bindBuffer(GLenum target) {
    if (hasBuffer) {
        glBindFramebuffer(target, texbuff);
        return !glCheckErr();
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
    return !glCheckErr();
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
    glCheckErr();
}

void vbaTex::setOobBehavior(GLint behavior) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, behavior);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, behavior);
    glCheckErr();
}
