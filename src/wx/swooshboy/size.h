#ifndef VBA_SIZE_H
#define VBA_SIZE_H

#include "swooshboy.h"
#include <GL/glxew.h>
#include <GL/glut.h>

/* Just a struct for holding sizes or coords or what-have-you */
typedef struct vbaSize {
    GLuint x;
    GLuint y;
} vbaSize;
// TODO: Turn vbaSize into a class that can be multiplied by integers

#endif
