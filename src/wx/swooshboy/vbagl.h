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
    friend class vbaOSD;
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
    vbaErr errGet();
    bool errPrint();

private:
    bool draw();

    /* pushErr/errVBASet
     * add a VBA error to the error queue
     *
     * The pushErr function takes the error value, source filename in which the
     * error is caught, line number where the error is caught, and function name
     * in which it was caught.
     *
     * The macro errVBASet takes the error value only.
     */
    void pushErr(vbaErrVal val, const char *file, int line, const char *func);
    #ifndef errVBASet
    #define errVBASet(err) pushErr(err, __FILE__, __LINE__, __func__)
    #endif

    /* pushErrFT/errFTSet
     * add an FT error to the error queue
     * Sets the VBA error to VBA_ERR_FT_ERR in the vbaErr objects.
     */
    void pushErrFT(FT_Error val, const char *file, int line, const char *func);
    #ifndef errFTSet
    #define errFTSet(err) pushErrFT(err, __FILE__, __LINE__, __func__)
    #endif

    /* pushErrGL/errGLCheck
     * Check for GL errors and add them to the error queue
     * Returns true if one or more GL errors are found.
     * Sets the VBA error to VBA_ERR_GL_ERR in the vbaErr objects.
     */
    bool pushErrGL(const char *file, int line, const char *func);
    #ifndef errGLCheck
    #define errGLCheck() pushErrGL(__FILE__, __LINE__, __func__)
    #endif

    /* pushErrGL/errGLCheckVBASet/errVBASetGLCheck
     * Check for GL errors and add them to the error queue.
     * Returns true if one or more GL errors are found.
     * Sets the VBA error to the provided VBA error value in the vbaErr objects.
     */
    bool pushErrGL(vbaErrVal val, const char *file, int line, const char *func);
    #ifndef errGLCheckVBASet
    #define errGLCheckVBASet(err) pushErrGL(err, __FILE__, __LINE__, __func__)
    #endif
    #ifndef errVBASetGLCheck
    #define errVBASetGLCheck(err) errGLCheckVBASet(err)
    #endif

    /* pushErrFT/errVBASetFTSet/ErrFTSetVBASet
     * add an FT error to the error queue
     * Sets the VBA error to the provided VBA error value in the vbaErr objects.
     */
    void pushErrFT(vbaErrVal val, FT_Error ftval, const char *file, int line,
                   const char *func);
    #ifndef errVBASetFTSet
    #define errVBASetFTSet(err, fterr) pushErrFT(err, fterr, __FILE__, \
                                                 __LINE__, __func__)
    #endif
    #ifndef errFTSetVBASet
    #define errFTSetVBASet(fterr, err) errVBASetFTSet(err, fterr)
    #endif

    /* catchErrGL/errGLCatch
     * Check for GL errors and add them to the error queue.
     * Returns true if one or more GL errors are found.
     * Ignores the first GL error if it matches a provided GL error value.
     * Sets the VBA error to VBA_ERR_GL_ERR in the vbaErr objects.
     */
    bool catchErrGL(GLenum ignore, const char *file, int line,
                    const char *func);
    #ifndef errGLCatch
    #define errGLCatch(val) catchErrGL(val, __FILE__, __LINE__, __func__)
    #endif

    /* catchErrGL/errGLCatchVBASet/errVBASetGLCatch
     * Check for GL errors and add them to the error queue.
     * Returns true if one or more GL errors are found.
     * Ignores the first GL error if it matches a provided GL error value.
     * Sets the VBA error to the provided VBA error value in the vbaErr objects.
     */
    bool catchErrGL(GLenum ignore, vbaErrVal val, const char *file, int line,
                    const char *func);
    #ifndef errGLCatchVBASet
    #define errGLCatchVBASet(val, err) catchErrGL(val, err, __FILE__, \
                                                  __LINE__, __func__)
    #endif
    #ifndef errVBASetGLCatch
    #define errVBASetGLCatch(err, val) errGLCatchVBASet(val, err)
    #endif

    vbaSize base_sz;
    vbaSize vwpt_sz;
    std::deque<vbaTex> textures;
    GLuint largest_scale;
    std::queue<vbaErr> vbaErrs;
    vbaOSD *osd;

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

    glslProg *glsl_passthrough;
    glslProg *glsl_supereagle;
    glslSrc glsl_common;
};

#endif
