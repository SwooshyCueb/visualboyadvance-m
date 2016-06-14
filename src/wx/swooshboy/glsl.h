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

    void setPass(uint n);
    void setSrcTexUnit(GLuint n);
    void setSrcSz(vbaSize sz);
    void setDstSz(vbaSize sz);

private:
    bool glPushErr(const char *file, int line, const char *func);
    #ifndef glCheckErr
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)
    #endif

    GLint getUniformPtr(const char *name);
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
        GLint src_tex;
        GLint src_sz;
        GLint dst_sz;
        GLint pass;
        GLint texcoord;
        GLint v_pos;
        GLint modVwMtx;
        GLint projMtx;
    } vars;
};

#endif
