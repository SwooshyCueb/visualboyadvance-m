#ifndef VBA_GLSL_H
#define VBA_GLSL_H
#include "swooshboy.h"
#include "swooshboy/glsl/dummy.fragment.glsl.h"

class glslSrc {
public:
    glslSrc();
    void loadSrc(const unsigned char *src_in, const uint len);
    #define LOAD_GLSL_SRC(obj, name) \
        obj.loadSrc(name##_glsl, name##_glsl_len)

private:
    const char *src_main;
    uint src_main_len;
};

class glslShader {
public:
    glslShader(vbaGL *globj);

    bool compile();
    bool printInfoLog();

private:
    bool glPushErr(const char *file, int line, const char *func);
    #ifndef glCheckErr
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)
    #endif

    GLuint shader;

    GLint compiled;

    glslSrc *src;
    vbaGL *ctx;
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
