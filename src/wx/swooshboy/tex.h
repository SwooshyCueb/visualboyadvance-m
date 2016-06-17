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
    void setShaderProg(glslProg *program);

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
    void pushErr(vbaErrVal val, const char *file, int line, const char *func);
    #ifndef errVBASet
    #define errVBASet(err) pushErr(err, __FILE__, __LINE__, __func__)
    #endif
    bool pushErrGL(const char *file, int line, const char *func);
    #ifndef errGLCheck
    #define errGLCheck() pushErrGL(__FILE__, __LINE__, __func__)
    #endif
    bool pushErrGL(vbaErrVal val, const char *file, int line, const char *func);
    #ifndef errGLCheckVBASet
    #define errGLCheckVBASet(err) pushErrGL(err, __FILE__, __LINE__, __func__)
    #endif
    #ifndef errVBASetGLCheck
    #define errVBASetGLCheck(err) errGLCheckVBASet(err)
    #endif
    bool catchErrGL(GLenum ignore, const char *file, int line,
                    const char *func);
    #ifndef errGLCatch
    #define errGLCatch(val) catchErrGL(val, __FILE__, __LINE__, __func__)
    #endif
    bool catchErrGL(GLenum ignore, vbaErrVal val, const char *file, int line,
                    const char *func);
    #ifndef errGLCatchVBASet
    #define errGLCatchVBASet(val, err) catchErrGL(val, err, __FILE__, \
                                                  __LINE__, __func__)
    #endif
    #ifndef errVBASetGLCatch
    #define errVBASetGLCatch(err, val) errGLCatchVBASet(val, err)
    #endif

    GLuint texture;
    GLuint unit;
    GLuint scale;
    GLint resizefilt;
    GLint blitmask;
    vbaSize size;

    bool hasBuffer = false;
    bool hasShader = false;

    GLuint fbo;

    vbaGL *ctx;
    glslProg *prog;
};

#endif
