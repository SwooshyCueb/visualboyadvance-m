#include <GL/glxew.h>
#include <GL/glut.h>
#include "render.h"
#include <cstdio>
#include <cstddef>

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

vbaSize vbaGL::base_sz;
vbaSize vbaGL::vwpt_sz;
vbaTex* vbaGL::t_init = NULL;

uint vbaTex::qty = 0;
uint vbaTex::largest_scale = 0;
GLenum vbaTex::DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};

bool vbaGL::initGL() {
    if (glewInit() != GLEW_OK)
        return false;
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
    glLoadIdentity();
    //glOrtho(0.0, 1.0, 1.0, 0.0, 0.0, 1.0);
    glOrtho(0.0, 1.0, 1.0, 0.0, 1.0, 0.0);
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

    return true;
}

bool vbaGL::draw() {
    #ifndef VBA_TRIANGLE_STRIP
    glDrawArrays(GL_QUADS, 0, 4);
    #else
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    #endif

    return true;
}

vbaTex::vbaTex(uint mult) {
    scale = mult;
    unit = qty;
    glGenTextures(1, &texture);
    setData(NULL);
    setResizeFilter(GL_NEAREST);
    setOobBehavior(GL_CLAMP_TO_EDGE);
    if (mult) {
        largest_scale = (scale > largest_scale) ? scale : largest_scale;
        glGenFramebuffers(1, &texbuff);
        glGenRenderbuffers(1, &rdrbuff);
        bindBuffer();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, rdrbuff);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, texture, 0);
        glDrawBuffers(1, DrawBuffers);
    } else {
        texbuff = 0;
    }
    qty++;
}

bool vbaTex::bind() {
    //glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    //bind(0);
    return true;
}

bool vbaTex::bind(uint num) {
    glActiveTexture(GL_TEXTURE0 + num);
    glBindTexture(GL_TEXTURE_2D, texture);
    return true;
}

bool vbaTex::bindBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, texbuff);
    return true;
}

bool vbaTex::bindBufferRead() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, texbuff);
    return true;
}

bool vbaTex::bindBufferWrite() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, texbuff);
    return true;
}

bool vbaTex::setData(const GLvoid *data) {
    vbaSize sz = getSize();
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sz.x, sz.y, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);
    return true;
}

vbaSize vbaTex::getSize(){
    vbaSize ret;
    if (!scale) {
        // Might use largest_scale here instead
        ret.x = vbaGL::vwpt_sz.x;
        ret.y = vbaGL::vwpt_sz.y;
    } else {
        ret.x = vbaGL::base_sz.x * scale;
        ret.y = vbaGL::base_sz.y * scale;
    }
    return ret;
}

void vbaTex::setResizeFilter(GLint filter) {
    resizefilt = filter;
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    if (filter = GL_NEAREST)
        blitmask = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    else
        blitmask = GL_COLOR_BUFFER_BIT;
}

void vbaTex::setOobBehavior(GLint behavior) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, behavior);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, behavior);
}
