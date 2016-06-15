#include "swooshboy.h"

// I know this is ugly. I plan on revamping this at some point.

#define VERT_MIN_F -1.0
#define VERT_MAX_F  1.0

#define VERT_F_0_0 VERT_MIN_F, VERT_MIN_F
#define VERT_F_0_1 VERT_MIN_F, VERT_MAX_F
#define VERT_F_1_0 VERT_MAX_F, VERT_MIN_F
#define VERT_F_1_1 VERT_MAX_F, VERT_MAX_F

#define VERT_MIN_I int(VERT_MIN_F)
#define VERT_MAX_I int(VERT_MAX_I)

#define VERT_I_0_0 VERT_MIN_I, VERT_MIN_I, 0
#define VERT_I_0_1 VERT_MIN_I, VERT_MAX_I, 0
#define VERT_I_1_0 VERT_MAX_I, VERT_MIN_I, 0
#define VERT_I_1_1 VERT_MAX_I, VERT_MAX_I, 0

#define COORD_MIN 0.0
#define COORD_MAX 1.0

#define COORD_0_0 COORD_MIN, COORD_MIN
#define COORD_0_1 COORD_MIN, COORD_MAX
#define COORD_1_0 COORD_MAX, COORD_MIN
#define COORD_1_1 COORD_MAX, COORD_MAX

#ifndef VBA_TRIANGLE_STRIP
GLfloat vbaGL::draw_vert[8] = {
    VERT_F_0_0,
    VERT_F_1_0,
    VERT_F_1_1,
    VERT_F_0_1,
};
GLfloat vbaGL::draw_coord[8] = {
    COORD_0_0,
    COORD_1_0,
    COORD_1_1,
    COORD_0_1,
};
#else
GLint vbaGL::draw_vert[12] = {
    VERT_I_0_0,
    VERT_I_1_0,
    VERT_I_0_1,
    VERT_I_1_1,
};
GLfloat vbaGL::draw_coord[8] = {
    COORD_0_0,
    COORD_1_0,
    COORD_0_1,
    COORD_1_1,
};
#endif

GLenum vbaGL::DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
