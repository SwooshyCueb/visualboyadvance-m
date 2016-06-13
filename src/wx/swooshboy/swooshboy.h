#ifndef VBA_SB_H
#define VBA_SB_H

#include <GL/glxew.h>
#include <GL/glut.h>
//#include <wx/glcanvas.h>

// Forward declarations to make life easier.
class glErr;
class vbaGL;
class vbaTex;

// Temporary
typedef struct vbaDrawArrs {
    #ifndef VBA_TRIANGLE_STRIP
    GLfloat vert[8];
    #else
    GLint vert[12];
    #endif
    GLfloat coord[8];
} vbaDrawArrs;

#include "glerr.h"
#include "sberr.h"
#include "size.h"
#include "vbagl.h"
#include "tex.h"

#include <cstdio>
#include <cstddef>
#include <deque>
#include <queue>
#include <stack>

//#define VBA_TRIANGLE_STRI

#ifdef _DEBUG
#define dprintf(format, ...) fprintf(stderr, format, ##__VA_ARGS__)
#else
#define dprintf(format, ...)
#endif


#endif
