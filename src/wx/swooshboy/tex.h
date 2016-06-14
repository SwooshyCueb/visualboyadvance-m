#ifndef VBA_TEX_H
#define VBA_TEX_H

#include "swooshboy.h"
#include <GL/glxew.h>
#include <GL/glut.h>
#include "glsl.h"

/* vbaTex
 * Texture object for use in vbaGL
 */
class vbaTex {
    friend class vbaGL;
public:
    vbaTex(uint mult, vbaGL *globj);
    ~vbaTex();

    void updSize();

    void setResizeFilter(GLint filter);
    void setOobBehavior(GLint behavior);

    bool setData(const GLvoid *data);

    bool bind();
    bool bind(uint num);

    bool initBuffer();
    bool remBuffer();

    bool bindBuffer(GLenum target);
    bool bindBuffer();
    bool bindBufferRead();
    bool bindBufferWrite();

private:
    bool glPushErr(const char *file, int line, const char *func);
    #ifndef glCheckErr
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)
    #endif

    GLuint texture;
    GLuint unit;
    GLuint scale;
    GLint resizefilt;
    GLint blitmask;
    vbaSize size;

    bool hasBuffer = false;

    GLuint fbo;

    vbaGL *ctx;
    glslProg *prog;
};

#endif
