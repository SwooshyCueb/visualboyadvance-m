#include <GL/glxew.h>
#include <GL/glut.h>
#include "oxygen.h"
#include "vbagl.h"
#include <glib.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif


static struct sigaction sigst;
static struct winsize termsz;
static void SIGWINCHhandler(int sig) {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &termsz);
    termwidth = termsz.ws_col;
}

vbaGL::vbaGL() {
    // The following code will be moved once we have rid ourselves of wx


    if(isatty(STDOUT_FILENO)) {
        sigemptyset(&sigst.sa_mask);
        sigst.sa_flags = 0;
        sigst.sa_handler = SIGWINCHhandler;
        sigaction(SIGWINCH, &sigst, NULL);

        ioctl(STDOUT_FILENO, TIOCGWINSZ, &termsz);
        termwidth = termsz.ws_col;

        vba_loglevel_strings = vba_loglevel_strings_256;
    } else {
        termwidth = 0;
        vba_loglevel_strings = vba_loglevel_strings_0;
    }
    // End of code to be moved

    log_debug("logging test", "test test");

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

    //osd = new vbaOSD(this);

    GLenum err = errGLCheck();
    if (err != GL_NO_ERROR) {
        errThrowGLVBA(err, VBA_ERR_GL_INIT);
    }
}

vbaGL::~vbaGL() {
    glDeleteBuffers(1, &vb_vtx);
    glDeleteBuffers(1, &vb_texcoord);
    glDeleteVertexArrays(1, &vtxArr);

    while(!vbaErrs.empty()) {
        vbaErrs.pop();
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

    if (init_p)
        pipeline.refreshStages();
}

void vbaGL::setVwptSize(uint x, uint y) {
    vwpt_sz.x = x;
    vwpt_sz.y = y;
    glVwpt(x, y);

    if (init_p)
        pipeline.refreshStages();
}

void vbaGL::setBaseScale(float scale) {
    base_scale = scale;
}

vbaSize vbaGL::getBaseSize() {
    vbaSize ret(base_sz);
    return ret;
}

vbaSize vbaGL::getVwptSize() {
    vbaSize ret(vwpt_sz);
    return ret;
}

float vbaGL::getBaseScale() {
    return base_scale;
}

bool vbaGL::glVwpt(vbaSize sz) {
    return glVwpt(sz.x, sz.y);
}

bool vbaGL::glVwpt(uint x, uint y) {
    glViewport(0, 0, x, y);
    return !errGLCheck();
}

bool vbaGL::render(const void *data) {
    pipeline.render(data);
    pipeline.draw();
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

/* Might do this differently */
bool vbaGL::initPipeline(uint scale) {
    base_scale = scale;

    pipeline.init(this);

    init_p = true;

    renderStage *passthru1 = new stgPassthrough(this);
    renderStage *supereagle1 = new stgSuperEagle(this);
    renderStage *passthru2 = new stgPassthrough(this);

    pipeline.addStage(passthru1);
    pipeline.addStage(supereagle1);
    pipeline.addStage(passthru2);

    return true;
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
