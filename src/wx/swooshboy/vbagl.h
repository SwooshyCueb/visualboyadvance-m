#ifndef VBA_GL_H
#define VBA_GL_H

#include "swooshboy.h"
#include <GL/glxew.h>
#include <GL/glut.h>
#include <deque>
#include <queue>
#include <stack>

/* vbaGL
 * OpenGL stuff.
 */
class vbaGL {
    friend class vbaTex;
public:
    vbaGL();
    ~vbaGL();

    void setBaseSize(uint x, uint y);
    void setVwptSize(uint x, uint y);

    bool genTextures(uint scale);
    bool setTexData(const GLvoid *data);

    bool glVwpt(uint x, uint y);
    bool glVwpt(vbaSize sz);

    vbaDrawArrs genDrawArrs(uint x, uint y);

    bool draw();
    void clear();
    glErr glErrGet();
    void glErrPrint();

private:
    bool glPushErr(const char *file, int line, const char *func);
    #ifndef glCheckErr
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)
    #endif

    vbaSize base_sz;
    vbaSize vwpt_sz;
    std::deque<vbaTex> textures;
    GLuint largest_scale;
    std::queue<glErr> glErrs;
    std::stack<vbaErr> vbaErrs;

    #ifndef VBA_TRIANGLE_STRIP
    static GLfloat draw_vert[8];
    #else
    static GLint draw_vert[12];
    #endif
    static GLfloat draw_coord[8];
    static GLenum DrawBuffers[1];
};

#endif
