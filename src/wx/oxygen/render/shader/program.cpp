#include "oxygen.h"
#include "program.h"

sharedGlProg::sharedGlProg() {
    program = glCreateProgram();
}

sharedGlProg::~sharedGlProg() {
    if (refcount) {
        errThrowVBA(VBA_ERR_DEINIT_OBJ_IN_USE);
    }

    detachShaders();
    glDeleteProgram(program);
}

bool sharedGlProg::attachShader(glslShader *shader) {
    glAttachShader(program, shader->shader);

    if (shader->type == GL_VERTEX_SHADER) {
        v = shader;
        has_vtx = true;
    } else if (shader->type == GL_FRAGMENT_SHADER) {
        f = shader;
        has_frag = true;
    }

    return true;
}

bool sharedGlProg::detachShaders() {
    if (has_vtx) {
        glDetachShader(program, v->shader);
        has_vtx = false;
    }

    if (has_frag) {
        glDetachShader(program, f->shader);
        has_vtx = false;
    }

    linked = 0;
    return true;
}

bool sharedGlProg::link() {
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    return true;
}

glslProg::glslProg() {
    is_init = false;
}

glslProg::glslProg(vbaGL *globj) {
    init(globj);
}

bool glslProg::init(vbaGL *globj) {
    if (is_init) {
        // Already initialized.
        return false;
    }
    ctx = globj;
    program = new sharedGlProg();
    errGLCheck();
    is_init = true;
    return true;
}

glslProg::~glslProg() {
    deinit();
}

bool glslProg::deinit() {
    if (is_init) {
        program->unref();
        is_init = false;
    }
    return true;
}

EH_DEFINE(glslProg);

bool glslProg::attachShader(glslShader *shader) {
    if (!is_init) {
        return false;
    }

    return program->attachShader(shader);
}

bool glslProg::detachShaders() {
    if (!is_init) {
        return false;
    }

    return program->detachShaders();
}

GLint glslProg::getUniformPtr(const char *name) {
    if (!is_init) {
        return false;
    }

    GLint ret = glGetUniformLocation(program->program, name);
    errGLCheck();
    if (ret < 0)
        log_debug("Shader handling", "Could not bind %s", name);
    return ret;
}

GLint glslProg::getAttrPtr(const char *name) {
    if (!is_init) {
        return false;
    }
    GLint ret = glGetAttribLocation(program->program, name);
    errGLCheck();
    if (ret < 0)
        log_debug("Shader handling", "Could not bind %s", name);
    return ret;
}

bool glslProg::enableVertAttrArr(const GLint arr) {
    if (!is_init) {
        return false;
    }
    if (arr < 0)
        return false;
    glEnableVertexAttribArray(arr);
    errGLCheck();
    return true;
}

bool glslProg::disableVertAttrArr(const GLint arr) {
    if (!is_init) {
        return false;
    }
    if (arr < 0)
        return false;
    glDisableVertexAttribArray(arr);
    errGLCheck();
    return true;
}

bool glslProg::setVtxAttrPtr(const GLint arr, GLint sz, GLenum typ,
                             GLboolean norm, GLsizei stride,
                             const GLvoid *ptr) {
    if (!is_init) {
        return false;
    }
    if (arr < 0)
        return false;
    glVertexAttribPointer(arr, sz, typ, norm, stride, ptr);
    errGLCheck();
    return true;
}

bool glslProg::link() {
    if (!is_init) {
        return false;
    }
    program->link();
    printInfoLog();

    if (!program->linked)
        return false;

    return !errGLCheck();
}

bool glslProg::activate() {
    if (!is_init) {
        return false;
    }
    glUseProgram(program->program);
    return !errGLCheck();
}

void glslProg::setVar1i(GLint var, GLint val) {
    if (!is_init) {
        return;
    }
    if (var < 0)
        return;

    GLint curr_prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog);
    glUseProgram(program->program);
    glUniform1i(var, val);
    glUseProgram(curr_prog);
    errGLCheck();
}

void glslProg::setVar2i(GLint var, GLint val1, GLint val2) {
    if (!is_init) {
        return;
    }
    if (var < 0)
        return;

    GLint curr_prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog);
    glUseProgram(program->program);
    glUniform2i(var, val1, val2);
    glUseProgram(curr_prog);
    errGLCheck();
}

void glslProg::setVar2f(GLint var, GLfloat val1, GLfloat val2) {
    if (!is_init) {
        return;
    }
    if (var < 0)
        return;

    GLint curr_prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog);
    glUseProgram(program->program);
    glUniform2f(var, val1, val2);
    glUseProgram(curr_prog);
    errGLCheck();
}

void glslProg::setVar4f(GLint var, GLfloat val1, GLfloat val2, GLfloat val3,
                        GLfloat val4) {
    if (!is_init) {
        return;
    }
    if (var < 0)
        return;

    GLint curr_prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog);
    glUseProgram(program->program);
    glUniform4f(var, val1, val2, val3, val4);
    glUseProgram(curr_prog);
    errGLCheck();
}

bool glslProg::printInfoLog() {
    if (!is_init) {
        return false;
    }
    GLint len = 0;
    GLint out = 0;
    GLchar *log;

    glGetProgramiv(program->program, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (GLchar *)malloc(len);
        if (log == NULL)
            return false;
        glGetProgramInfoLog(program->program, len, &out, log);
        log_info("Program InfoLog", "%s\n", log);
        free(log);
    }
    return true;
}

bool glslProg::shallowCopy(const glslProg &other) {
    if (is_init) {
        log_warning_debug("Shader handling",
                          "shallowCopy called on initialized glslProg.");
        deinit();
    }

    if (!other.is_init) {
        //No need to go any further
        return true;
    }

    is_init = other.is_init;
    ctx = other.ctx;

    program = other.program;
    program->ref();

    return true;
}

bool glslProg::deepCopy(const glslProg &other) {
    if (is_init) {
        log_warning_debug("Shader handling",
                          "deepCopy called on initialized glslProg.");
        deinit();
    }

    if (!other.is_init) {
        //No need to go any further
        return true;
    }

    is_init = other.is_init;
    ctx = other.ctx;

    program = new sharedGlProg();

    if (other.program->has_vtx) {
        attachShader(other.program->v);
    }

    if (other.program->has_frag) {
        attachShader(other.program->f);
    }

    if (other.program->linked) {
        link();
    }

    return true;
}

glslProg::glslProg(const glslProg &other) {
    log_warning_debug("Shader handling",
                      "Using copy constructor on glslProg object.\n"
                      "You probably don't want to do this.");

    #ifndef PROG_COPYCONS_SHALLOW
    log_debug("Shader handling", "Copying program by relinking.");
    deepCopy(other);
    #else
    log_debug("Shader handling", "Both glslProg objects will point to the same program.");
    shallowCopy(other);
    #endif

}

glslProg &glslProg::operator  = (const glslProg &other) {
    log_warning_debug("Shader handling",
                      "Using assignment operator on glslProg object.\n"
                      "You probably don't want to do this.");

    #ifndef PROG_ASSIGN_SHALLOW
    log_debug("Shader handling", "Copying program by relinking.");
    deepCopy(other);
    #else
    log_debug("Shader handling", "Both glslProg objects will point to the same program.");
    shallowCopy(other);
    #endif

    return *this;
}
