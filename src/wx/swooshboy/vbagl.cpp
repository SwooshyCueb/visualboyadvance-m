#include <GL/glxew.h>
#include <GL/glut.h>
#include "swooshboy.h"
#include "vbagl.h"
#include "glsl.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

vbaGL::vbaGL() {
    if (glewInit() != GLEW_OK) {
        errThrowVBA(VBA_ERR_GL_INIT);
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);

    glGenVertexArrays(1, &vtxArr);
    glBindVertexArray(vtxArr);

    glGenBuffers(1, &vb_vtx);
    glBindBuffer(GL_ARRAY_BUFFER, vb_vtx);
    #ifndef VBA_TRIANGLE_STRIP
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), draw_vert, GL_STATIC_DRAW);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    #else
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLint), draw_vert, GL_STATIC_DRAW);
    glVertexPointer(3, GL_INT, 0, 0);
    #endif

    glGenBuffers(1, &vb_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, vb_texcoord);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), draw_coord, GL_STATIC_DRAW);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    osd = new vbaOSD(this);

    GLenum err = errGLCheck();
    if (err != GL_NO_ERROR) {
        errThrowGLVBA(err, VBA_ERR_GL_INIT);
    }
}

/* pushErr/errVBASet
 * add a VBA error to the error queue
 *
 * The pushErr function takes the error value, source filename in which the
 * error is caught, line number where the error is caught, and function name
 * in which it was caught.
 *
 * The macro errVBASet takes the error value only.
 */
inline void vbaGL::pushErr(vbaErrVal val, const char *file, int line,
                           const char *func) {
    vbaErrs.emplace(val, file, line, func);
}

/* pushErrFT/errFTSet
 * add an FT error to the error queue
 * Sets the VBA error to VBA_ERR_FT_ERR in the vbaErr objects.
 */
inline void vbaGL::pushErrFT(FT_Error ftval, const char *file, int line,
                           const char *func) {
    vbaErrs.emplace(VBA_ERR_FT_ERR, ftval, file, line, func);
}

/* pushErrFT/errVBASetFTSet/ErrFTSetVBASet
 * add an FT error to the error queue
 * Sets the VBA error to the provided VBA error value in the vbaErr objects.
 */
inline void vbaGL::pushErrFT(vbaErrVal val, FT_Error ftval, const char *file,
                           int line, const char *func) {
    vbaErrs.emplace(val, ftval, file, line, func);
}

/* pushErrGL/errGLCheckVBASet/errVBASetGLCheck
 * Check for GL errors and add them to the error queue.
 * Returns true if one or more GL errors are found.
 * Sets the VBA error to the provided VBA error value in the vbaErr objects.
 */
inline bool vbaGL::pushErrGL(vbaErrVal val, const char *file, int line,
                             const char *func) {
    GLenum glErr;
    bool ret = false;

    while ((glErr = glGetError()) != GL_NO_ERROR) {
        vbaErrs.emplace(val, glErr, file, line, func);
        ret = true;
    }

    return ret;
}

/* pushErrGL/errGLCheck
 * Check for GL errors and add them to the error queue
 * Returns true if one or more GL errors are found.
 * Sets the VBA error to VBA_ERR_GL_ERR in the vbaErr objects.
 */
inline bool vbaGL::pushErrGL(const char *file, int line, const char *func) {
    return pushErrGL(VBA_ERR_GL_ERR, file, line, func);
}

/* catchErrGL/errGLCatch
 * Check for GL errors and add them to the error queue.
 * Returns true if one or more GL errors are found.
 * Ignores the first GL error if it matches a provided GL error value.
 * Sets the VBA error to VBA_ERR_GL_ERR in the vbaErr objects.
 */
inline bool vbaGL::catchErrGL(GLenum ignore, vbaErrVal val, const char *file,
                              int line, const char *func) {
    GLenum glErr;
    bool ret = false;
    glErr = glGetError();

    if (glErr == GL_NO_ERROR)
        return ret;

    if (glErr != ignore) {
        vbaErrs.emplace(val, glErr, file, line, func);
        ret = true;
    }

    while ((glErr = glGetError()) != GL_NO_ERROR) {
        vbaErrs.emplace(val, glErr, file, line, func);
        ret = true;
    }

    return ret;
}

/* catchErrGL/errGLCatchVBASet/errVBASetGLCatch
 * Check for GL errors and add them to the error queue.
 * Returns true if one or more GL errors are found.
 * Ignores the first GL error if it matches a provided GL error value.
 * Sets the VBA error to the provided VBA error value in the vbaErr objects.
 */
inline bool vbaGL::catchErrGL(GLenum ignore, const char *file, int line,
                              const char *func) {
    return catchErrGL(ignore, VBA_ERR_GL_ERR, file, line, func);
}

vbaErr vbaGL::errGet() {
    if (vbaErrs.size()) {
        vbaErr ret;
        ret = vbaErrs.front();
        vbaErrs.pop();
        return ret;
    } else {
        errThrowVBA(VBA_ERR_NO_MORE);
    }
}

bool vbaGL::errPrint() {
    uint errs = vbaErrs.size();
    bool ret = bool(errs);
    for (uint i = 0; i < errs; i++) {
        vbaErrs.front().print();
        vbaErrs.pop();
    }
    return ret;
}

void vbaGL::setBaseSize(uint x, uint y) {
    base_sz.x = x;
    base_sz.y = y;

    for (uint i = 0; i < textures.size(); i++) {
        textures[i].updSize();
    }
}

void vbaGL::setVwptSize(uint x, uint y) {
    vwpt_sz.x = x;
    vwpt_sz.y = y;
    glVwpt(x, y);

    for (uint i = 0; i < textures.size(); i++) {
        textures[i].updSize();
    }
}

inline bool vbaGL::glVwpt(vbaSize sz) {
    return glVwpt(sz.x, sz.y);
}

inline bool vbaGL::glVwpt(uint x, uint y) {
    glViewport(0, 0, x, y);
    return !errGLCheck();
}

bool vbaGL::render() {
    for (uint i = 0; i < textures.size(); i++) {
        textures[i].bind(textures[i].unit);
        if (textures[i].hasShader) {
            textures[i].prog->activate();
            textures[i].prog->setPassIdx(i+1);
            textures[i].prog->setSrcTexUnit(textures[i].unit);
            textures[i].prog->setDstSz(
                        (i != textures.size() - 1)
                        ? textures[i+1].size
                        : vwpt_sz
            );
            textures[i].prog->setNeedsFlip(
                        (i == textures.size() - 1)
                        ? true
                        : false
            );
            textures[i].prog->setSrcSz(textures[i].size);

        }
        if (i != textures.size() - 1) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, textures[i].size.x + 1);
            textures[i+1].bindBuffer();
            glVwpt(textures[i+1].size);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glVwpt(vwpt_sz);
        }
        draw();
        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    return !errGLCheck();
}

bool vbaGL::draw() {
    #ifndef VBA_TRIANGLE_STRIP
    glDrawArrays(GL_QUADS, 0, 4);
    #else
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    #endif
    return !errGLCheck();
}

void vbaGL::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

bool vbaGL::initShaders() {
    shd_passthru1 = new shdPassthrough(this);
    shd_supereagle = new shdSuperEagle(this);
    return true;
}

/* Might do this differently */
bool vbaGL::genTextures(uint scale) {
    // Initial texture
    textures.emplace_back(scale, this);
    textures.back().setShaderProg(shd_passthru1);

    // Intermediate texture 1
    textures.emplace_back(scale, this);
    textures.back().initBuffer();
    textures.back().setShaderProg(shd_supereagle);

    // Intermediate texture 2
    textures.emplace_back(scale * 2, this);
    textures.back().initBuffer();
    textures.back().setShaderProg(shd_passthru1);

    // Final texture
    textures.emplace_back(0, this);
    textures.back().initBuffer();
    textures.back().setShaderProg(shd_passthru1);

    // For now, pass_qty will just be the number of textures we have
    for (uint i = 0; i < textures.size(); i++) {
        if (textures[i].hasShader)
            textures[i].prog->setPassQty(textures.size());
    }

    return true;
}

/* Might do this differently */
bool vbaGL::setTexData(const GLvoid *data) {
    return textures.front().setData(data);
}

bool vbaGL::setVsyncState(int vsync) {
    // non-portable vsync code
#if defined(__linux__) && defined(GLX_SGI_swap_control)
    static PFNGLXSWAPINTERVALSGIPROC si = NULL;

    if (!si)
        si = (PFNGLXSWAPINTERVALSGIPROC)glXGetProcAddress((const GLubyte*)"glxSwapIntervalSGI");

    if (si)
        si(vsync);

    #elif defined(_WIN32) && defined(WGL_EXT_swap_control)
    static PFNWGLSWAPINTERVALEXTPROC si = NULL;

    if (!si)
        si = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

    if (si)
        si(vsync);

    #elif defined(__APPLE__) && !defined(TARGET_OS_IPHONE)
    int swap_interval = vsync ? 1 : 0;
    CGLContextObj cgl_context = CGLGetCurrentContext();
    CGLSetParameter(cgl_context, kCGLCPSwapInterval, &swap_interval);
    #else
    //#warning no vsync support on this platform
    return false;
    #endif
    return true;
}
