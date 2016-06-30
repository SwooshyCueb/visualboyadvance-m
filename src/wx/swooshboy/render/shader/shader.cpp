#include "swooshboy.h"
#include "shader.h"

// Somehow move this to header
GLchar *glslShader::glsl_version = "#version 100\n\0"; //14
GLchar *glslShader::glsl_defines_global = "#define DEBUG\n#define IN_VBA\n\0"; //30
GLchar *glslShader::glsl_defines_vert = "#define VERTEX\n\0"; //16
GLchar *glslShader::glsl_defines_frag = "#define FRAGMENT\n\0"; //18

glslShader::glslShader() {
    is_init = false;
}

glslShader::glslShader(vbaGL *globj, GLenum type_in) {
    init(globj, type_in);
}

bool glslShader::init(vbaGL *globj, GLenum type_in) {
    ctx = globj;
    type = type_in;
    shader = glCreateShader(type);
    GLenum err = errGLCheck();
    if (err != GL_NO_ERROR) {
        errThrowGLVBA(err, VBA_ERR_GL_ERR);
    }
    is_init = true;

    return true;
}

glslShader::~glslShader() {
    deinit();
}

bool glslShader::deinit() {
    if (is_init) {
        glDeleteShader(shader);
        is_init = false;
    }
    return true;
}

bool glslShader::setSrc(glslSrc *srcobj) {
    if (!is_init) {
        return false;
    }
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
    src_set = true;
    return !errGLCheck();
}

EH_DEFINE(glslShader);

bool glslShader::compile() {
    if (!is_init) {
        return false;
    }
    glCompileShader(shader);
    errGLCheck();
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    printInfoLog();

    return bool(compiled);
}

bool glslShader::printInfoLog() {
    if (!is_init) {
        return false;
    }
    GLint len = 0;
    GLint out = 0;
    GLchar *log;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (GLchar *)malloc(len);
        if (log == NULL)
            return false;
        glGetShaderInfoLog(shader, len, &out, log);
        log_info("Shader InfoLog", "%s\n", log);
        free(log);
    }
    return true;
}

bool glslShader::copy(const glslShader &other) {
    if (is_init) {
        log_warning_debug("Shader handling",
                          "copy called on initialized glslShader.");
        deinit();
    }

    if (!other.is_init) {
        // No need to go any further.
        return true;
    }

    commonCopy(other);
    if(src_set)
        glShaderSource(shader, 5, glsl, glsl_len);
    shader = glCreateShader(type);
    if (other.compiled) {
        log_warning_debug("Shader handling",
                          "Copying an already compiled shader.");
        compile();
    }
    return true;
}

glslShader::glslShader(const glslShader &other) {
    log_warning_debug("Shader handling",
                      "Using copy constructor on glslShader object.\n"
                      "You probably don't want to do this.");
    copy(other);
}

glslShader &glslShader::operator = (const glslShader &other) {
    log_warning_debug("Shader handling",
                      "Using assignment operator on glslShader object.\n"
                      "You probably don't want to do this.");
    copy(other);

    return *this;
}

// This will likely need to be reworked when we implement precompiler stuff
void glslShader::commonCopy(const glslShader &other) {
    is_init = other.is_init;
    type = other.type;
    ctx = ctx;
    src_set = other.src_set;

    src = other.src;
    glsl[0] = other.glsl[0];
    glsl[1] = other.glsl[1];
    glsl[2] = other.glsl[2];
    glsl[3] = other.glsl[3];
    glsl[4] = other.glsl[4];

    glsl_len[0] = other.glsl_len[0];
    glsl_len[1] = other.glsl_len[1];
    glsl_len[2] = other.glsl_len[2];
    glsl_len[3] = other.glsl_len[3];
    glsl_len[4] = other.glsl_len[4];
}
