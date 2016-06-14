#include "swooshboy.h"
#include "glsl.h"

GLchar *glslShader::glsl_version = "#version 100\n\0"; //14
GLchar *glslShader::glsl_defines_global = "#define DEBUG\n\0"; //15
GLchar *glslShader::glsl_defines_vert = "#define VERTEX\n\0"; //16
GLchar *glslShader::glsl_defines_frag = "#define FRAGMENT\n\0"; //18

glslSrc::glslSrc() {
    src_main = NULL;
    src_main_len = 0;
}

void glslSrc::loadSrc(unsigned char *src_in, const uint len) {
    src_main = (char*)src_in;
    src_main_len = len;
}

glslShader::glslShader(vbaGL *globj, GLenum type_in) {
    ctx = globj;
    type = type_in;
    shader = glCreateShader(type);
    if (glCheckErr())
        throw VBAERR_GLERR;
}

bool glslShader::setSrc(glslSrc *srcobj) {
    src = srcobj;

    glsl[0] = glsl_version;
    glsl_len[0] = 14;

    glsl[1] = glsl_defines_global;
    glsl_len[1] = 15;

    glsl[4] = src->src_main;
    glsl_len[4] = src->src_main_len;

    if (type == GL_VERTEX_SHADER) {
        glsl[2] = glsl_defines_vert;
        glsl_len[2] = 16;
    } else if (type == GL_FRAGMENT_SHADER) {
        glsl[2] = glsl_defines_frag;
        glsl_len[2] = 18;
    }

    glsl[3] = "\0";
    glsl_len[3] = 0;

    glShaderSource(shader, 5, glsl, glsl_len);
    return !glCheckErr();
}

inline bool glslShader::glPushErr(const char *file, int line, const char *func) {
    return ctx->glPushErr(file, line, func);
}

bool glslShader::compile() {
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    printInfoLog();

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
