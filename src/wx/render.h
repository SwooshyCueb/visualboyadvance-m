#ifndef VBA_RENDER_H
#define VBA_RENDER_H

#include <GL/glxew.h>
#include <GL/glut.h>
#include <wx/glcanvas.h>
#include <cstdio>
#include <cstddef>

//#define VBA_TRIANGLE_STRIP

class vbaGL {
public:
    static bool initGL();
    static bool draw();
private:
    #ifndef VBA_TRIANGLE_STRIP
    static GLfloat draw_vert[8];
    #else
    static GLint draw_vert[12];
    #endif
    static GLfloat draw_coord[8];
};

#endif
