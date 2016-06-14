#ifndef VBA_SIZE_H
#define VBA_SIZE_H

#include "swooshboy.h"
#include <GL/glxew.h>
#include <GL/glut.h>

class vbaSize {
public:
    vbaSize();
    vbaSize(uint w, uint h);
    vbaSize(uint w,  int h);
    vbaSize( int w, uint h);
    vbaSize( int w,  int h);
    vbaSize(const vbaSize &other);

    vbaSize &operator  = (const vbaSize &other);
    vbaSize &operator += (const vbaSize &other);
    vbaSize &operator -= (const vbaSize &other);
    vbaSize &operator *= (const uint &num);
    vbaSize &operator *= (const  int &num);
    vbaSize &operator /= (const uint &num);
    vbaSize &operator /= (const  int &num);
    vbaSize &operator %= (const uint &num);
    vbaSize &operator %= (const  int &num);

    GLuint x;
    GLuint y;
};

#endif
