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

    vars.texcoord = getAttrPtr("texcoord");
    vars.v_pos = getAttrPtr("v_pos");
    vars.modVwMtx = getUniformPtr("modVwMtx");
    vars.src_tex = getUniformPtr("src_tex");
    vars.src_sz = getUniformPtr("src_sz");
    vars.dst_sz = getUniformPtr("dst_sz");
    vars.pass = getUniformPtr("pass");
    glCheckErr();

    if(hasVtx) {
        activate();
        glBindVertexArray(ctx->vtxArrVtx);
        glBindBuffer(GL_ARRAY_BUFFER, ctx->vtxBuffVtx);
        #ifndef VBA_TRIANGLE_STRIP
        setVtxAttrPtr(vars.v_pos, 2, GL_FLOAT, GL_FALSE, 0, 0);
        #else
        setVtxAttrPtr(vars.v_pos, 3, GL_INT, GL_FALSE, 0, 0);
        #endif
        enableVertAttrArr(vars.v_pos);

        glBindVertexArray(ctx->vtxArrTexCoord);
        glBindBuffer(GL_ARRAY_BUFFER, ctx->vtxBuffTexCoord);
        setVtxAttrPtr(vars.texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
        enableVertAttrArr(vars.texcoord);
        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return !glCheckErr();
}

bool glslProg::activate() {
    glUseProgram(program);
    return !glCheckErr();
}

void glslProg::setPass(uint n) {
    if (vars.pass < 0)
        return;
    glUniform1i(vars.pass, n);
    glCheckErr();
}

void glslProg::setSrcTexUnit(GLuint n) {
    if (vars.src_tex < 0)
        return;
    glUniform1i(vars.src_tex, GLint(n));
    glCheckErr();
}

void glslProg::setSrcSz(vbaSize sz) {
    if (vars.src_sz < 0)
        return;
    glUniform2f(vars.src_sz, sz.x, sz.y);
    glCheckErr();
}

void glslProg::setDstSz(vbaSize sz) {
    if (vars.dst_sz < 0)
        return;
    glUniform2f(vars.dst_sz, sz.x, sz.y);
    glCheckErr();
}

void glslProg::updMatrices() {
    GLfloat model[16];
    //GLfloat proj[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, model);
    glCheckErr();
    //glGetFloatv(GL_PROJECTION, proj);
    //glCheckErr();
    glUniformMatrix4fv(vars.modVwMtx, 1, GL_FALSE, model);
    glCheckErr();
    //glUniformMatrix4fv(vars.projMtx, 1, GL_FALSE, proj);
    //glCheckErr();
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
