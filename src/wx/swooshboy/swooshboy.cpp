#include "swooshboy.h"

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
