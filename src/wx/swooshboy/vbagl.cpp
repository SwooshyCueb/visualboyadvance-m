#include <GL/glxew.h>
#include <GL/glut.h>
#include "swooshboy.h"
#include "vbagl.h"
#include "glsl.h"

vbaGL::vbaGL() {
    if (glewInit() != GLEW_OK) {
        vbaErrs.push(VBAERR_GLINIT);
        throw VBAERR_GLINIT;
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);

    glGenVertexArrays(1, &vtxArr);
    glBindVertexArray(vtxArr);

    glGenBuffers(1, &vb_vtx);
    glBindBuffer(GL_ARRAY_BUFFER, vb_vtx);
    #ifndef VBA_TRIANGLE_STRIP
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), draw_vert, GL_STATIC_DRAW);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    #else
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLint), draw_vert, GL_STATIC_DRAW);
    glVertexPointer(3, GL_INT, 0, 0);
    #endif

    glGenBuffers(1, &vb_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, vb_texcoord);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), draw_coord, GL_STATIC_DRAW);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (glCheckErr()) {
        throw VBAERR_GLERR;
    }
}

inline bool vbaGL::glPushErr(const char *file, int line, const char *func) {
    GLenum val;
    bool ret = false;

    while ((val = glGetError()) != GL_NO_ERROR) {
        glErrs.emplace(val, file, line - 1, func);
        vbaErrs.push(VBAERR_GLERR);
        ret = true;
    }
    return ret;
}

inline bool vbaGL::glPushErr(const char *file, int line, const char *func, GLenum err) {
    GLenum val;
    bool ret = false;
    //glErr *err;
    val = glGetError();

    if ((val == GL_NO_ERROR) || (val == err))
        return ret;

    do {
        glErrs.emplace(val, file, line - 1, func);
        vbaErrs.push(VBAERR_GLERR);
        ret = true;
    } while ((val = glGetError()) != GL_NO_ERROR);
    return ret;
}

glErr vbaGL::glErrGet() {
    if (glErrs.size()) {
        glErr ret;
        ret = glErrs.front();
        glErrs.pop();
        return ret;
    } else {
        throw VBAERR_NODATA;
    }
}

bool vbaGL::glErrPrint() {
    uint errs = glErrs.size();
    bool ret = bool(errs);
    for (uint i = 0; i < errs; i++) {
        glErrs.front().print();
        glErrs.pop();
    }
    return ret;
}

void vbaGL::setBaseSize(uint x, uint y) {
    base_sz.x = x;
    base_sz.y = y;

    for (uint i = 0; i < textures.size(); i++) {
        textures[i].updSize();
    }
}

void vbaGL::setVwptSize(uint x, uint y) {
    vwpt_sz.x = x;
    vwpt_sz.y = y;
    glVwpt(x, y);

    for (uint i = 0; i < textures.size(); i++) {
        textures[i].updSize();
    }
}

inline bool vbaGL::glVwpt(vbaSize sz) {
    return glVwpt(sz.x, sz.y);
}

inline bool vbaGL::glVwpt(uint x, uint y) {
    glViewport(0, 0, x, y);
    return !glCheckErr();
}

bool vbaGL::draw() {
    for (uint i = 0; i < textures.size(); i++) {
        textures[i].bind(textures[i].unit);
        if (textures[i].hasShader) {
            textures[i].prog->activate();
            textures[i].prog->setPassIdx(i+1);
            textures[i].prog->setSrcTexUnit(textures[i].unit);
            textures[i].prog->setDstSz(
                        (i != textures.size() - 1)
                        ? textures[i+1].size
                        : vwpt_sz
            );
            textures[i].prog->setNeedsFlip(
                        (i == textures.size() - 1)
                        ? true
                        : false
            );
            textures[i].prog->setSrcSz(textures[i].size);

        }
        if (i != textures.size() - 1) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, textures[i].size.x + 1);
            textures[i+1].bindBuffer();
            glVwpt(textures[i+1].size);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glVwpt(vwpt_sz);
        }
        #ifndef VBA_TRIANGLE_STRIP
        glDrawArrays(GL_QUADS, 0, 4);
        #else
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        #endif
        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glCheckErr();
    }
    return !glCheckErr();
}

void vbaGL::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

bool vbaGL::initShaders() {
    glslSrc dummy_src;
    LOAD_GLSL_SRC(dummy_src, dummy);
    glslShader dummy_shd_f(this, GL_FRAGMENT_SHADER);
    glslShader dummy_shd_v(this, GL_VERTEX_SHADER);
    dummy_shd_f.setSrc(&dummy_src);
    dummy_shd_f.compile();
    dummy_shd_v.setSrc(&dummy_src);
    dummy_shd_v.compile();

    dummyglsl = new glslProg(this);
    dummyglsl->attachShader(dummy_shd_f);
    dummyglsl->attachShader(dummy_shd_v);
    dummyglsl->init();
    return true;
}

/* Might do this differently */
bool vbaGL::genTextures(uint scale) {
    // Initial texture
    textures.emplace_back(scale, this);
    textures.back().setShaderProg(dummyglsl);

    // Intermediate texture 1
    textures.emplace_back(scale, this);
    textures.back().initBuffer();
    textures.back().setShaderProg(dummyglsl);

    // Intermediate texture 2
    textures.emplace_back(scale, this);
    textures.back().initBuffer();
    textures.back().setShaderProg(dummyglsl);

    // Final texture
    textures.emplace_back(0, this);
    textures.back().initBuffer();
    textures.back().setShaderProg(dummyglsl);

    // For now, pass_qty will just be the number of textures we have
    for (uint i = 0; i < textures.size(); i++) {
        if (textures[i].hasShader)
            textures[i].prog->setPassQty(textures.size());
    }

    return true;
}

/* Might do this differently */
bool vbaGL::setTexData(const GLvoid *data) {
    return textures.front().setData(data);
}
