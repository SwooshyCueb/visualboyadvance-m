#ifndef VBA_RENDER_H
#define VBA_RENDER_H

#include <GL/glxew.h>
#include <GL/glut.h>
#include <wx/glcanvas.h>
#include <cstdio>
#include <cstddef>

//#define VBA_TRIANGLE_STRIP

typedef struct vbaSize {
    GLuint x;
    GLuint y;
} vbaSize;

class vbaTex {
public:
    vbaTex(uint mult);
    ~vbaTex();
    void setResizeFilter(GLint filter);
    vbaSize getSize();
    bool bind();
    bool bind(uint num);
    bool bindBuffer();
    bool bindBufferRead();
    bool bindBufferWrite();
    void setOobBehavior(GLint behavior);
    bool setData(const GLvoid *data);
private:
    GLuint texture;
    GLuint unit;
    GLuint texbuff;
    GLuint rdrbuff;
    GLuint scale;
    GLint resizefilt;
    GLint blitmask;

    static uint qty;
    static uint largest_scale;
    static GLenum DrawBuffers[1];
};

class vbaGL {
public:
    static bool initGL();
    static bool draw();
    static vbaSize base_sz;
    static vbaSize vwpt_sz;

    static vbaTex *t_init;
private:
    #ifndef VBA_TRIANGLE_STRIP
    static GLfloat draw_vert[8];
    #else
    static GLint draw_vert[12];
    #endif
    static GLfloat draw_coord[8];
};

#endif
