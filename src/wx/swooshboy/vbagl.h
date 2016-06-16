#ifndef VBA_GL_H
#define VBA_GL_H

#include "swooshboy.h"
#include "glsl.h"
#include <GL/glxew.h>
#include <GL/glut.h>
#include <deque>
#include <queue>
#include <stack>

// Moving these here for my reference
/*
#define int_fmt out_16 ? GL_RGB5 : GL_RGB
#define tex_fmt out_16 ? GL_BGRA : GL_RGBA, \
                out_16 ? GL_UNSIGNED_SHORT_1_5_5_5_REV : GL_UNSIGNED_BYTE
*/

/* vbaGL
 * OpenGL stuff.
 */
class vbaGL {
    friend class vbaTex;
    friend class glslSrc;
    friend class glslShader;
    friend class glslProg;
public:
    vbaGL();
    ~vbaGL();

    void setBaseSize(uint x, uint y);
    void setVwptSize(uint x, uint y);

    bool genTextures(uint scale);
    bool setTexData(const GLvoid *data);
    bool setVsyncState(int vsync);

    bool initShaders();

    bool glVwpt(uint x, uint y);
    bool glVwpt(vbaSize sz);

    bool render();
    void clear();
    glErr glErrGet();
    bool glErrPrint();

private:
    bool draw();

    bool glPushErr(const char *file, int line, const char *func);
    #ifndef glCheckErr
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)
    #endif
    bool glPushErr(const char *file, int line, const char *func, GLenum err);
    #ifndef glIgnoreErr
    #define glIgnoreErr(err) glPushErr(__FILE__, __LINE__, __func__, err);
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

    GLfloat* mtx_ortho;

    GLuint vtxArr;
    GLuint vb_vtx;
    GLuint vb_texcoord;

    glslProg *dummyglsl;
};

#endif
