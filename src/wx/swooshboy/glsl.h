#ifndef VBA_GLSL_H
#define VBA_GLSL_H
#include "swooshboy.h"
#include "swooshboy/glsl/dummy.fragment.glsl.h"

class glslSrc {
    friend class glslShader;
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
public:
    glslProg(vbaGL *globj);

    bool printInfoLog();

private:
    bool glPushErr(const char *file, int line, const char *func);
    #ifndef glCheckErr
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)
    #endif

    GLuint program;

    vbaGL *ctx;
};

#endif
