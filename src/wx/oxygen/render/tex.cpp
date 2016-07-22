#include "oxygen.h"
#include "tex.h"

vbaTex::vbaTex() {
    is_init = false;
}

vbaTex::vbaTex(vbaGL *globj, vbaSize sz) {
    init(globj, sz);
}

vbaTex::vbaTex(vbaGL *globj, uint x, uint y) {
    init(globj, x, y);
}

vbaTex::vbaTex(vbaGL *globj, vbaSize sz, GLenum fmt) {
    init(globj, sz, fmt);
}

vbaTex::vbaTex(vbaGL *globj, uint x, uint y, GLenum fmt) {
    init(globj, x, y, fmt);
}

bool vbaTex::init(vbaGL *globj, vbaSize sz) {
    return init(globj, sz, DEFAULT_TEX_FMT);
}

bool vbaTex::init(vbaGL *globj, uint x, uint y) {
    return init(globj, x, y, DEFAULT_TEX_FMT);
}

bool vbaTex::init(vbaGL *globj, vbaSize sz, GLenum fmt) {
    ctx = globj;
    size = sz;
    format = fmt;
    glGenTextures(1, &texture);
    is_init = true;
    setData(NULL);
    setResizeFilter(GL_NEAREST);
    setOobBehavior(GL_CLAMP_TO_EDGE);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    return true;
}

bool vbaTex::init(vbaGL *globj, uint x, uint y, GLenum fmt) {
    ctx = globj;
    format = fmt;
    size.x(x);
    size.y(y);
    glGenTextures(1, &texture);
    is_init = true;
    setData(NULL);
    setResizeFilter(GL_NEAREST);
    setOobBehavior(GL_CLAMP_TO_EDGE);
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

bool vbaTex::bind() {
    if (!is_init) {
        return false;
    }
    //glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    //bind(0);
    return !errGLCheck();
}

bool vbaTex::bind(uint num) {
    if (!is_init) {
        return false;
    }
    glActiveTexture(GL_TEXTURE0 + num);
    glBindTexture(GL_TEXTURE_2D, texture);
    return !errGLCheck();
}

bool vbaTex::setData(const GLvoid *data) {
    if (!is_init) {
        return false;
    }
    bind();
    glPixelStorei(GL_UNPACK_ROW_LENGTH, size.xu() + 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, size.xu(), size.yu(), 0, format,
                 GL_UNSIGNED_BYTE, data);
    return !errGLCheck();
}

void vbaTex::setSize(vbaSize sz) {
    if (!is_init) {
        return;
    }
    size = sz;
}

void vbaTex::setSize(uint x, uint y) {
    if (!is_init) {
        return;
    }
    size.x(x);
    size.y(y);
}

void vbaTex::setResizeFilter(GLint filter) {
    if (!is_init) {
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
    if (!is_init) {
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
