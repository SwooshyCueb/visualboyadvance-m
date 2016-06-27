#include "swooshboy.h"
#include "program.h"

glslProg::glslProg() {
    is_init = false;
}

glslProg::glslProg(vbaGL *globj) {
    init(globj);
}

bool glslProg::init(vbaGL *globj) {
    ctx = globj;
    program = glCreateProgram();
    errGLCheck();
    has_vtx = has_frag = false;
    is_init = true;
    return true;
}

glslProg::~glslProg() {
    deinit();
}

bool glslProg::deinit() {
    if (is_init) {
        if (has_vtx) {
            glDetachShader(program, v->shader);
            has_vtx = false;
        }

        if (has_frag) {
            glDetachShader(program, f->shader);
            has_vtx = false;
        }

        glDeleteProgram(program);
        is_init = false;
    }
    return true;
}

EH_DEFINE(glslProg);

bool glslProg::attachShader(glslShader *shader) {
    if (!is_init) {
        return false;
    }
    glAttachShader(program, shader->shader);
    errGLCheck();
    if (shader->type == GL_VERTEX_SHADER) {
        v = shader;
        has_vtx = true;
    } else if (shader->type == GL_FRAGMENT_SHADER) {
        f = shader;
        has_frag = true;
    }
    return !errGLCheck();
}

inline GLint glslProg::getUniformPtr(const char *name) {
    if (!is_init) {
        return false;
    }
    GLint ret = glGetUniformLocation(program, name);
    errGLCheck();
    if (ret < 0)
        dprintf("Could not bind %s\n", name);
    return ret;
}

inline GLint glslProg::getAttrPtr(const char *name) {
    if (!is_init) {
        return false;
    }
    GLint ret = glGetAttribLocation(program, name);
    errGLCheck();
    if (ret < 0)
        dprintf("Could not bind %s\n", name);
    return ret;
}

inline bool glslProg::enableVertAttrArr(const GLint arr) {
    if (!is_init) {
        return false;
    }
    if (arr < 0)
        return false;
    glEnableVertexAttribArray(arr);
    errGLCheck();
    return true;
}

inline bool glslProg::disableVertAttrArr(const GLint arr) {
    if (!is_init) {
        return false;
    }
    if (arr < 0)
        return false;
    glDisableVertexAttribArray(arr);
    errGLCheck();
    return true;
}

inline bool glslProg::setVtxAttrPtr(const GLint arr, GLint sz, GLenum typ,
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
    glLinkProgram(program);
    errGLCheck();
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    errGLCheck();

    printInfoLog();

    if (!linked)
        return false;

    if (has_vtx) {
        vars.v.position = getAttrPtr("v_pos");
        vars.v.texcoord = getAttrPtr("v_texcoord");

        vars.v.src_sz = getUniformPtr("v_src_sz");
        vars.v.dst_sz = getUniformPtr("v_dst_sz");

        vars.v.pass_idx = getUniformPtr("v_pass_idx");
        vars.v.pass_qty = getUniformPtr("v_pass_qty");
    }

    if (has_frag) {
        vars.f.src_tex = getUniformPtr("src_tex");

        vars.f.src_sz = getUniformPtr("f_src_sz");
        vars.f.dst_sz = getUniformPtr("f_dst_sz");

        vars.f.pass_idx = getUniformPtr("f_pass_idx");
        vars.f.pass_qty = getUniformPtr("f_pass_qty");
    }

    vars.needs_flip = getUniformPtr("needs_flip");

    errGLCheck();

    if(has_vtx) {
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

    return !errGLCheck();
}

bool glslProg::activate() {
    if (!is_init) {
        return false;
    }
    glUseProgram(program);
    return !errGLCheck();
}

inline void glslProg::setVar1i(GLint var, GLint val) {
    if (!is_init) {
        return;
    }
    if (var < 0)
        return;

    GLint curr_prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog);
    glUseProgram(program);
    glUniform1i(var, val);
    glUseProgram(curr_prog);
    errGLCheck();
}

inline void glslProg::setVar2i(GLint var, GLint val1, GLint val2) {
    if (!is_init) {
        return;
    }
    if (var < 0)
        return;

    GLint curr_prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog);
    glUseProgram(program);
    glUniform2i(var, val1, val2);
    glUseProgram(curr_prog);
    errGLCheck();
}

inline void glslProg::setVar2f(GLint var, GLfloat val1, GLfloat val2) {
    if (!is_init) {
        return;
    }
    if (var < 0)
        return;

    GLint curr_prog;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curr_prog);
    glUseProgram(program);
    glUniform2f(var, val1, val2);
    glUseProgram(curr_prog);
    errGLCheck();
}

void glslProg::setPassQty(uint n) {
    if (!is_init) {
        return;
    }
    setVar1i(vars.v.pass_qty, n);
    setVar1i(vars.f.pass_qty, n);
}

void glslProg::setPassIdx(uint n) {
    if (!is_init) {
        return;
    }
    setVar1i(vars.v.pass_idx, n);
    setVar1i(vars.f.pass_idx, n);
}

void glslProg::setSrcTexUnit(GLuint n) {
    if (!is_init) {
        return;
    }
    setVar1i(vars.f.src_tex, GLint(n));
}

void glslProg::setSrcSz(vbaSize sz) {
    if (!is_init) {
        return;
    }
    setVar2f(vars.v.src_sz, sz.x, sz.y);
    setVar2f(vars.f.src_sz, sz.x, sz.y);
}

void glslProg::setDstSz(vbaSize sz) {
    if (!is_init) {
        return;
    }
    setVar2f(vars.v.dst_sz, sz.x, sz.y);
    setVar2f(vars.f.dst_sz, sz.x, sz.y);
}

void glslProg::setNeedsFlip(bool flip) {
    if (!is_init) {
        return;
    }
    setVar1i(vars.needs_flip, flip ? 1 : 0);
}

bool glslProg::printInfoLog() {
    if (!is_init) {
        return false;
    }
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

bool glslProg::shallowCopy(const glslProg &other) {
    if (is_init) {
        dprintf("WARN: shallowCopy called on initialized glslProg.");
        deinit();
    }

    if (!other.is_init) {
        //No need to go any further
        return true;
    }

    is_init = other.is_init;
    ctx = other.ctx;

    has_vtx = other.has_vtx;
    v = other.v;

    has_frag = other.has_frag;
    f = other.f;

    program = other.program;
    linked = other.linked;

    vars.needs_flip = other.vars.needs_flip;
    vars.v.position = other.vars.v.position;
    vars.v.texcoord = other.vars.v.texcoord;
    vars.v.src_sz =   other.vars.v.src_sz;
    vars.v.dst_sz =   other.vars.v.dst_sz;
    vars.v.pass_idx = other.vars.v.pass_idx;
    vars.v.pass_qty = other.vars.v.pass_qty;
    vars.f.src_sz =   other.vars.f.src_sz;
    vars.f.dst_sz =   other.vars.f.dst_sz;
    vars.f.pass_idx = other.vars.f.pass_idx;
    vars.f.pass_qty = other.vars.f.pass_qty;
    vars.f.src_tex =  other.vars.f.src_tex;

    return true;
}

bool glslProg::deepCopy(const glslProg &other) {
    if (is_init) {
        dprintf("WARN: deepCopy called on initialized glslProg.");
        deinit();
    }

    if (!other.is_init) {
        //No need to go any further
        return true;
    }

    is_init = other.is_init;
    ctx = other.ctx;

    program = glCreateProgram();

    if (other.has_vtx) {
        attachShader(other.v);
    }

    if (other.has_frag) {
        attachShader(other.f);
    }

    if (other.linked) {
        link();
    }

}

glslProg::glslProg(const glslProg &other) {
    dprintf("WARN: ");
    dprintf("Using copy constructor on glslProg object.\n");
    dprintf("\tYou probably don't want to do this.\n");

    #ifndef PROG_COPYCONS_SHALLOW
    dprintf("Copying program by relinking.");
    deepCopy(other);
    #else
    dprintf("Both glslProg objects will point to the same program.\n");
    shallowCopy(other);
    #endif


}

glslProg &glslProg::operator  = (const glslProg &other) {
    dprintf("WARN: ");
    dprintf("Using assignment operator on glslProg object.\n");
    dprintf("\tYou probably don't want to do this.\n");

    #ifndef PROG_ASSIGN_SHALLOW
    dprintf("Copying program by relinking.");
    deepCopy(other);
    #else
    dprintf("Both glslProg objects will point to the same program.\n");
    shallowCopy(other);
    #endif

    return *this;
}
