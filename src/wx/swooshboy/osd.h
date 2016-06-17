#ifndef VBA_OSD_H
#define VBA_OSD_H

#include "swooshboy.h"
#include "vbagl.h"
#include "tex.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <deque>

class ftGlyph {
public:
    char character;

    vbaSize sz_tex;
    vbaSize sz_glyph;

    unsigned char *data;
    FT_Vector adv;
};

class vbaOSD {
    friend class vbaGL;
public:
    vbaOSD(vbaGL *globj);
    ~vbaOSD();

private:
    void getGlyph(char character);

    void pushErr(vbaErrVal val, const char *file, int line, const char *func);
    #ifndef errVBASet
    #define errVBASet(err) pushErr(err, __FILE__, __LINE__, __func__)
    #endif
    void pushErrFT(FT_Error val, const char *file, int line, const char *func);
    #ifndef errFTSet
    #define errFTSet(err) pushErrFT(err, __FILE__, __LINE__, __func__)
    #endif
    bool pushErrGL(const char *file, int line, const char *func);
    #ifndef errGLCheck
    #define errGLCheck() pushErrGL(__FILE__, __LINE__, __func__)
    #endif
    void pushErrFT(vbaErrVal val, FT_Error ftval, const char *file, int line,
                   const char *func);
    #ifndef errVBASetFTSet
    #define errVBASetFTSet(err, fterr) pushErrFT(err, fterr, __FILE__, \
                                                 __LINE__, __func__)
    #endif
    #ifndef errFTSetVBASet
    #define errFTSetVBASet(fterr, err) errVBASetFTSet(err, fterr)
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

    bool init = false;

    struct {
        FT_Library lib;
        FT_Face face;
        FT_GlyphSlot gs;
    } ft;

    std::deque<ftGlyph> glyphs;
    ftGlyph currGlyph;
    ftGlyph placeholder;

    vbaGL *ctx;

};

#endif
