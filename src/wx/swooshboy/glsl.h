#ifndef VBA_GLSL_H
#define VBA_GLSL_H
#include "swooshboy.h"
#include "size.h"
#include "swooshboy/glsl/dummy.glsl.h"

class glslSrc {
    friend class glslShader;
    friend class vbaGL;
public:
    glslSrc();
    void loadSrc(unsigned char *src_in, const uint len);
    #define LOAD_GLSL_SRC(obj, name) \
        obj.loadSrc(const_cast<unsigned char *>(name##_glsl), name##_glsl_len)

private:
    char *src_main;
    uint src_main_len;
};

class glslShader {
    friend class glslProg;
    friend class vbaGL;
public:
    glslShader(vbaGL *globj, GLenum type_in);
    bool setSrc(glslSrc *srcobj);

    bool compile();
    bool printInfoLog();

private:
    bool glPushErr(const char *file, int line, const char *func);
    #ifndef glCheckErr
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)
    #endif
    bool glPushErr(const char *file, int line, const char *func, GLenum err);
    #ifndef glIgnoreErr
    #define glIgnoreErr(err) glPushErr(__FILE__, __LINE__, __func__, err);
    #endif

    GLuint shader;

    GLint compiled;
    GLenum type;

    glslSrc *src;
    GLchar *glsl[5];
    GLint glsl_len[5];
    vbaGL *ctx;

    static GLchar *glsl_version;
    static GLchar *glsl_defines_global;
    static GLchar *glsl_defines_vert;
    static GLchar *glsl_defines_frag;
};

class glslProg {
    friend class vbaGL;
public:
    glslProg(vbaGL *globj);

    bool printInfoLog();
    bool attachShader(glslShader shader);

    bool init();
    bool activate();

    void setPassQty(uint n);
    void setPassIdx(uint n);
    void setSrcTexUnit(GLuint n);
    void setSrcSz(vbaSize sz);
    void setDstSz(vbaSize sz);
    void setNeedsFlip(bool flip);

private:
    bool glPushErr(const char *file, int line, const char *func);
    #ifndef glCheckErr
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)
    #endif
    bool glPushErr(const char *file, int line, const char *func, GLenum err);
    #ifndef glIgnoreErr
    #define glIgnoreErr(err) glPushErr(__FILE__, __LINE__, __func__, err);
    #endif

    GLint getUniformPtr(const char *name);
    void setVar1i(GLint var, GLint val);
    void setVar2f(GLint var, GLfloat val1, GLfloat val2);

    GLint getAttrPtr(const char *name);
    bool enableVertAttrArr(const GLint arr);
    bool disableVertAttrArr(const GLint arr);
    bool setVtxAttrPtr(const GLint arr, GLint sz, GLenum typ, GLboolean norm,
                       GLsizei stride, const GLvoid *ptr);

    void updMatrices();

    GLuint program;
    GLint linked;
    bool hasVtx = false;
    bool hasFrag = false;

    vbaGL *ctx;

    glslShader *v;
    glslShader *f;

    struct {
        struct {
            GLint position = 0;
            GLint texcoord = 0;

            GLint src_sz = 0;
            GLint dst_sz = 0;

            GLint pass_idx = 0;
            GLint pass_qty = 0;
        } v;

        struct {
            GLint src_tex = 0;

            GLint src_sz = 0;
            GLint dst_sz = 0;

            GLint pass_idx = 0;
            GLint pass_qty = 0;
        } f;

        GLint needs_flip = 0; // Can be in either, but not both

    } vars;
};

#endif
