/******************************************************************************/
/* err.h                                                                      */
/******************************************************************************/
/* This header is modelled after fterrors.h from the FreeType2 project.       */
/* It contains macros used to define the handling of our error values, as     */
/* as our error handler class. It can also be used to generate error message  */
/* strings, as seen in err.cpp.                                               */
/******************************************************************************/

#ifndef VBA_ERR_H
#define VBA_ERR_H

#include "swooshboy.h"
#include <GL/glxew.h>
#include <GL/glut.h>

#include <ft2build.h>
#include FT_FREETYPE_H


#ifndef VBAERR_PREFIX
#define VBAERR_PREFIX VBA_ERR_
#endif

#ifndef VBAERR_TYPE
typedef unsigned long int vbaErrVal;
#define VBAERR_TYPE
#endif

#ifndef GL_ERR_TYPE
typedef GLenum glErrVal;
#define GL_ERR_TYPE
#endif

#ifndef VBAERR_DEF
#define VBAERR_DEF(code, string, value) code = value,
#define VBAERR_START_LIST enum : unsigned long int {
#define VBAERR_END_LIST };
#endif

#define VBAERR_CAT_(prefix, error) prefix ## error
#define VBAERR_CAT(prefix, error) VBAERR_CAT_(prefix, error)

#define VBAERR_DEF_(code, string, value) \
    VBAERR_DEF(VBAERR_CAT(VBAERR_PREFIX, code), string, value)

#ifdef VBAERR_START_LIST
VBAERR_START_LIST
#endif

#include "vbaerr_defs.h"

#ifdef VBAERR_END_LIST
VBAERR_END_LIST
#endif

#ifndef VBAERR_CLASS
/* vbaErr
 * Error info object
 *
 * I know this is probably a way overcomplicated way of doing this.
 * You have my most sincere (but not really heartfelt) apologies.
 *
 * As well, I fear I may have /created/ a memory leak or two in trying to
 * avoid them. Feedback in this aspect is most certainly welcome.
 */
class vbaErr {
public:
    /* Dummy constructor */
    vbaErr();

    /* Standard constructor
     *
     * Takes a VBA error enum, the filename of the source file the error came
     * from, the line number where the error was caught, and the function in
     * which it was caught.
     */
    vbaErr(vbaErrVal val, const char *src, uint loc, const char *fnc);

    /* GL error constructor
     *
     * Takes a VBA error enum, a GL error enum, the filename of the source file
     * the error came from, the line number where the error was caught, and the
     * function in which it was caught.
     */
    vbaErr(vbaErrVal val, GLenum glval, const char *src, uint loc,
           const char *fnc);

    /* FreeType error constructor
     *
     * Takes a VBA error enum, a FreeType error enum, the filename of the source
     * file the error came from, the line number where the error was caught, and
     * the function in which it was caught.
     */
    vbaErr(vbaErrVal val, FT_Error ftval, const char *src, uint loc,
           const char *fnc);

    /* FreeType+GL error constructor
     *
     * Takes a VBA error enum, a GL error enum, a FreeType error enum, the
     * filename of the source file the error came from, the line number where
     * the error was caught, and the function in which it was caught.
     */
    vbaErr(vbaErrVal val, GLenum glval, FT_Error ftval, const char *src,
           uint loc, const char *fnc);

    /* Setters
     *
     * Sets the instance's values as if they were constructed with the given
     * arguments. (This, and the = operator, are how you initialize an instance
     * constructed with the dummy constructor
     */
    void set(vbaErrVal val, const char *src, uint loc, const char *fnc);
    void set(vbaErrVal val, GLenum glval, const char *src, uint loc,
             const char *fnc);
    void set(vbaErrVal val, FT_Error ftval, const char *src, uint loc,
             const char *fnc);
    void set(vbaErrVal val, GLenum glval, FT_Error ftval, const char *src,
           uint loc, const char *fnc);

    /* Destructor
     *
     * Does what it says on the tin.
     */
    ~vbaErr();


    /* Error printing function
     *
     * Prints information about the error to stderr.
     */
    void print();

    /* = operator
     *
     * Does what it says on the tin
     */
    vbaErr& operator = (const vbaErr &other);

//private:
    static const char* vbaGetStr(vbaErrVal val);
    static const char* ftGetStr(FT_Error val);

    vbaErrVal err = VBA_ERR_OK; //VBA error value
    GLenum glErr = GL_NO_ERROR; //GL error value
    FT_Error ftErr = FT_Err_Ok; //FreeType error value
    char *file; //source filename
    char *func; //function name
    uint line; //line number
    bool init = false; //is the instance actually initalized?
};

#define errThrowVBA(err) throw vbaErr(err, __FILE__, __LINE__, __func__)

#define errThrowVBAGL(err, glErr) throw vbaErr(err, glErr, __FILE__, __LINE__, \
                                               __func__)
#define errThrowVBAFT(err, ftErr) throw vbaErr(err, ftErr, __FILE__, __LINE__, \
    __func__)

#define errThrowVBAGLFT(err, glErr, ftErr) throw vbaErr(err, glErr, ftErr, \
                                                        __FILE__, __LINE__, \
                                                        __func__)

#define errThrowFTVBA(ftErr, err) errThrowVBAFT(err, ftErr)
#define errThrowGLVBA(glErr, err) errThrowVBAGL(err, glErr)

#define errThrowVBAFTGL(err, ftErr, glErr) errThrowVBAGLFT(err, glErr, ftErr)
#define errThrowFTVBAGL(ftErr, err, glErr) errThrowVBAGLFT(err, glErr, ftErr)
#define errThrowFTGLVBA(ftErr, glErr, err) errThrowVBAGLFT(err, glErr, ftErr)
#define errThrowGLVBAFT(glErr, err, ftErr) errThrowVBAGLFT(err, glErr, ftErr)
#define errThrowGLFTVBA(glErr, ftErr, err) errThrowVBAGLFT(err, glErr, ftErr)

#define errThrowGL(glErr) errThrowVBAGL(VBA_ERR_GL_ERR, glErr)
#define errThrowFT(ftErr) errThrowVBAFT(VBA_ERR_FT_ERR, ftErr)
#endif


#ifdef VBAERR_START_LIST
#undef VBAERR_START_LIST
#endif

#ifdef VBAERR_END_LIST
#undef VBAERR_END_LIST
#endif

#ifdef VBAERR_DEF
#undef VBAERR_DEF
#endif

#ifdef VBAERR_DEF_
#undef VBAERR_DEF_
#endif

#define EH_DEFINE(objn) \
    inline void objn::pushErr(vbaErrVal val, const char *file, int line, \
                              const char *func) { \
        return ctx->pushErr(val, file, line, func); \
    } \
    inline void objn::pushErrFT(FT_Error ftval, const char *file, int line, \
                                const char *func) { \
        return ctx->pushErr(ftval, file, line, func); \
    } \
    inline void objn::pushErrFT(vbaErrVal val,FT_Error ftval, \
                                const char *file, int line, \
                                const char *func) { \
        return ctx->pushErr(val, file, line, func); \
    } \
    inline bool objn::pushErrGL(const char *file, int line, \
                                const char *func) { \
        return ctx->pushErrGL(file, line, func); \
    } \
    inline bool objn::pushErrGL(vbaErrVal val, const char *file, int line, \
                                const char *func) { \
        return ctx->pushErrGL(val, file, line, func); \
    } \
    inline bool objn::catchErrGL(GLenum ignore, const char *file, int line, \
                                 const char *func) { \
        return ctx->catchErrGL(ignore, file, line, func); \
    } \
    inline bool objn::catchErrGL(GLenum ignore, vbaErrVal val, \
                                 const char *file, int line, \
                                 const char *func) { \
        return ctx->catchErrGL(ignore, val, file, line, func); \
    } \
    static int just_here_to_allow_semicolon##objn __attribute__((unused))

#define errVBASet(err) pushErr(err, __FILE__, __LINE__, __func__)
#define errFTSet(err) pushErrFT(err, __FILE__, __LINE__, __func__)
#define errVBASetFTSet(err, fterr) pushErrFT(err, fterr, __FILE__, \
                                             __LINE__, __func__)
#define errFTSetVBASet(fterr, err) errVBASetFTSet(err, fterr)
#define errGLCheck() pushErrGL(__FILE__, __LINE__, __func__)
#define errGLCheckVBASet(err) pushErrGL(err, __FILE__, __LINE__, __func__)
#define errVBASetGLCheck(err) errGLCheckVBASet(err)
#define errGLCatch(val) catchErrGL(val, __FILE__, __LINE__, __func__)
#define errGLCatchVBASet(val, err) catchErrGL(val, err, __FILE__, \
                                              __LINE__, __func__)
#define errVBASetGLCatch(err, val) errGLCatchVBASet(val, err)

#endif

#ifndef EH_DECLARE
#define EH_DECLARE() \
    void pushErr(vbaErrVal val, const char *file, int line, const char *func); \
    void pushErrFT(FT_Error val, const char *file, int line, \
                   const char *func); \
    void pushErrFT(vbaErrVal val, FT_Error ftval, const char *file, int line, \
                   const char *func); \
    bool pushErrGL(const char *file, int line, const char *func); \
    bool pushErrGL(vbaErrVal val, const char *file, int line, \
                   const char *func); \
    bool catchErrGL(GLenum ignore, const char *file, int line, \
                    const char *func); \
    bool catchErrGL(GLenum ignore, vbaErrVal val, const char *file, int line, \
                    const char *func)
#endif
