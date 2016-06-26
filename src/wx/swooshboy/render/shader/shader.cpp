#include "swooshboy.h"
#include "shader.h"

GLchar *glslShader::glsl_version = "#version 100\n\0"; //14
GLchar *glslShader::glsl_defines_global = "#define DEBUG\n#define IN_VBA\n\0"; //30
GLchar *glslShader::glsl_defines_vert = "#define VERTEX\n\0"; //16
GLchar *glslShader::glsl_defines_frag = "#define FRAGMENT\n\0"; //18

glslShader::glslShader(vbaGL *globj, GLenum type_in) {
    ctx = globj;
    type = type_in;
    shader = glCreateShader(type);
    GLenum err = errGLCheck();
    if (err != GL_NO_ERROR) {
        errThrowGLVBA(err, VBA_ERR_GL_ERR);
    }
}

bool glslShader::setSrc(glslSrc *srcobj) {
    src = srcobj;

    glsl[0] = glsl_version;
    glsl_len[0] = 13;

    glsl[1] = glsl_defines_global;
    glsl_len[1] = 29;

    if (type == GL_VERTEX_SHADER) {
        glsl[2] = glsl_defines_vert;
        glsl_len[2] = 15;
    } else if (type == GL_FRAGMENT_SHADER) {
        glsl[2] = glsl_defines_frag;
        glsl_len[2] = 17;
    }

    glsl[3] = ctx->glsl_common.src_main;
    glsl_len[3] = ctx->glsl_common.src_main_len;

    glsl[4] = src->src_main;
    glsl_len[4] = src->src_main_len;

    glShaderSource(shader, 5, glsl, glsl_len);
    return !errGLCheck();
}

EH_DEFINE(glslShader);

bool glslShader::compile() {
    glCompileShader(shader);
    errGLCheck();
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
