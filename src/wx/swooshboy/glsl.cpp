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
    glsl_len[0] = 13;

    glsl[1] = glsl_defines_global;
    glsl_len[1] = 14;

    glsl[4] = src->src_main;
    glsl_len[4] = src->src_main_len;

    if (type == GL_VERTEX_SHADER) {
        glsl[2] = glsl_defines_vert;
        glsl_len[2] = 15;
    } else if (type == GL_FRAGMENT_SHADER) {
        glsl[2] = glsl_defines_frag;
        glsl_len[2] = 17;
    }

    glsl[3] = "\0";
    glsl_len[3] = 0;

    glShaderSource(shader, 5, glsl, glsl_len);
    return !glCheckErr();
}

inline bool glslShader::glPushErr(const char *file, int line, const char *func) {
    return ctx->glPushErr(file, line, func);
}
inline bool glslShader::glPushErr(const char *file, int line, const char *func, GLenum err) {
    return ctx->glPushErr(file, line, func, err);
}

bool glslShader::compile() {
    glCompileShader(shader);
    glCheckErr();
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
    program = glCreateProgram();
    glCheckErr();
    hasVtx = hasFrag = false;
}

inline bool glslProg::glPushErr(const char *file, int line, const char *func) {
    return ctx->glPushErr(file, line, func);
}
inline bool glslProg::glPushErr(const char *file, int line, const char *func, GLenum err) {
    return ctx->glPushErr(file, line, func, err);
}

bool glslProg::attachShader(glslShader shader) {
    glAttachShader(program, shader.shader);
    glCheckErr();
    if (shader.type == GL_VERTEX_SHADER) {
        v = &shader;
        hasVtx = true;
    } else if (shader.type == GL_FRAGMENT_SHADER) {
        f = &shader;
        hasFrag = true;
    }
    return !glCheckErr();
}

inline GLint glslProg::getUniformPtr(const char *name) {
    GLint ret = glGetUniformLocation(program, name);
    glCheckErr();
    if (ret < 0)
        dprintf("Could not bind %s\n", name);
    return ret;
}

inline GLint glslProg::getAttrPtr(const char *name) {
    GLint ret = glGetAttribLocation(program, name);
    glCheckErr();
    if (ret < 0)
        dprintf("Could not bind %s\n", name);
    return ret;
}

inline bool glslProg::enableVertAttrArr(const GLint arr) {
    if (arr < 0)
        return false;
    glEnableVertexAttribArray(arr);
    glCheckErr();
    return true;
}

inline bool glslProg::disableVertAttrArr(const GLint arr) {
    if (arr < 0)
        return false;
    glDisableVertexAttribArray(arr);
    glCheckErr();
    return true;
}

inline bool glslProg::setVtxAttrPtr(const GLint arr, GLint sz, GLenum typ,
                                    GLboolean norm, GLsizei stride,
                                    const GLvoid *ptr) {
    if (arr < 0)
        return false;
    glVertexAttribPointer(arr, sz, typ, norm, stride, ptr);
    glCheckErr();
    return true;
}

bool glslProg::init() {
    glLinkProgram(program);
    glCheckErr();
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    glCheckErr();

    printInfoLog();

    if (!linked)
        return false;

    if (hasVtx) {
        vars.v.position = getAttrPtr("v_pos");
        vars.v.texcoord = getAttrPtr("texcoord");

        vars.v.src_sz = getUniformPtr("v_src_sz");
        vars.v.dst_sz = getUniformPtr("v_dst_sz");

        vars.v.pass_idx = getUniformPtr("v_pass_idx");
        vars.v.pass_qty = getUniformPtr("v_pass_qty");
    }

    if (hasFrag) {
        vars.f.src_tex = getUniformPtr("src_tex");

        vars.f.src_sz = getUniformPtr("f_src_sz");
        vars.f.dst_sz = getUniformPtr("f_dst_sz");

        vars.f.pass_idx = getUniformPtr("f_pass_idx");
        vars.f.pass_qty = getUniformPtr("f_pass_qty");
    }

    vars.needs_flip = getUniformPtr("needs_flip");

    glCheckErr();

    if(hasVtx) {
        activate();
        glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_vtx);
        #ifndef VBA_TRIANGLE_STRIP
        setVtxAttrPtr(vars.v.position, 2, GL_FLOAT, GL_FALSE, 0, 0);
        #else
        setVtxAttrPtr(vars.v.position, 3, GL_INT, GL_FALSE, 0, 0);
        #endif
        enableVertAttrArr(vars.v.position);

        glBindBuffer(GL_ARRAY_BUFFER, ctx->vb_texcoord);
        setVtxAttrPtr(vars.v.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
        enableVertAttrArr(vars.v.texcoord);

        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return !glCheckErr();
}

bool glslProg::activate() {
    glUseProgram(program);
    return !glCheckErr();
}

inline void glslProg::setVar1i(GLint var, GLint val) {
    if (var < 0)
        return;

    GLint curr_prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog);
    glUseProgram(program);
    glUniform1i(var, val);
    glUseProgram(curr_prog);
    glCheckErr();
}

inline void glslProg::setVar2f(GLint var, GLfloat val1, GLfloat val2) {
    if (var < 0)
        return;

    GLint curr_prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog);
    glUseProgram(program);
    glUniform2f(var, val1, val2);
    glUseProgram(curr_prog);
    glCheckErr();
}

void glslProg::setPassQty(uint n) {
    setVar1i(vars.v.pass_qty, n);
    setVar1i(vars.f.pass_qty, n);
}

void glslProg::setPassIdx(uint n) {
    setVar1i(vars.v.pass_idx, n);
    setVar1i(vars.f.pass_idx, n);
}

void glslProg::setSrcTexUnit(GLuint n) {
    setVar1i(vars.f.src_tex, GLint(n));
}

void glslProg::setSrcSz(vbaSize sz) {
    setVar2f(vars.v.src_sz, sz.x, sz.y);
    setVar2f(vars.f.src_sz, sz.x, sz.y);
}

void glslProg::setDstSz(vbaSize sz) {
    setVar2f(vars.v.dst_sz, sz.x, sz.y);
    setVar2f(vars.f.dst_sz, sz.x, sz.y);
}

void glslProg::setNeedsFlip(bool flip) {
    setVar1i(vars.needs_flip, flip ? 1 : 0);
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
