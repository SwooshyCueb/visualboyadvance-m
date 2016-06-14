#ifndef VBA_GLSL_H
#define VBA_GLSL_H
#include "swooshboy.h"

class glslSrc {
public:
    glslSrc(vbaGL *globj);

private:
    bool glPushErr(const char *file, int line, const char *func);
    #ifndef glCheckErr
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)
    #endif

    vbaGL *ctx;
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
