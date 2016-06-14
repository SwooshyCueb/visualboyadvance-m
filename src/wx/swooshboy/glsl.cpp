#include "swooshboy.h"
#include "glsl.h"

glslSrc::glslSrc(vbaGL *globj) {
    ctx = globj;
}

inline bool glslSrc::glPushErr(const char *file, int line, const char *func) {
    return ctx->glPushErr(file, line, func);
}

glslShader::glslShader(vbaGL *globj) {
    ctx = globj;
}

inline bool glslShader::glPushErr(const char *file, int line, const char *func) {
    return ctx->glPushErr(file, line, func);
}

bool glslShader::compile() {
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    return bool(compiled);
}

bool glslShader::printInfoLog() {
    GLint len = 0;
    GLint out = 0;
    GLchar *log;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (GLchar *)malloc(len);
        if (log == NULL)
            return false;
        glGetShaderInfoLog(shader, len, &out, log);
        dprintf("Shader InfoLog:\n%s\n\n", log);
        free(log);
    }
    return true;
}

glslProg::glslProg(vbaGL *globj) {
    ctx = globj;
}

inline bool glslProg::glPushErr(const char *file, int line, const char *func) {
    return ctx->glPushErr(file, line, func);
}

bool glslProg::printInfoLog() {
    GLint len = 0;
    GLint out = 0;
    GLchar *log;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (GLchar *)malloc(len);
        if (log == NULL)
            return false;
        glGetProgramInfoLog(program, len, &out, log);
        dprintf("Program InfoLog:\n%s\n\n", log);
        free(log);
    }
    return true;
}
