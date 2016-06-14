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
    glMatrixMode(GL_PROJECTION);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 1.0, 0.0, 0.0, 1.0);
    //glOrtho(0.0, 1.0, 1.0, 0.0, 1.0, 0.0);
    //glMatrixMode(GL_TEXTURE);
    //glLoadIdentity();
    //glScalef(1.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);

    #ifndef VBA_TRIANGLE_STRIP
    glVertexPointer(2, GL_FLOAT, 0, draw_vert);
    #else
    glVertexPointer(3, GL_INT, 0, draw_vert);
    #endif
    glTexCoordPointer(2, GL_FLOAT, 0, draw_coord);

    if (glCheckErr()) {
        throw VBAERR_GLERR;
    }
}

inline bool vbaGL::glPushErr(const char *file, int line, const char *func) {
    GLenum val;
    bool ret = false;
    //glErr *err;

    while ((val = glGetError()) != GL_NO_ERROR) {
        //err = new glErr(val, file, line - 1, func);
        //glErrs.push(err);
        glErrs.emplace(val, file, line - 1, func);
        vbaErrs.push(VBAERR_GLERR);
        ret = true;
    }
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

vbaDrawArrs vbaGL::genDrawArrs(uint x, uint y) {
    vbaDrawArrs ret;

    #ifndef VBA_TRIANGLE_STRIP
    ret.vert[0] = 0.0;      ret.vert[1] = 0.0;
    ret.vert[2] = float(x); ret.vert[3] = 0.0;
    ret.vert[4] = float(x); ret.vert[5] = float(y);
    ret.vert[6] = 0.0;      ret.vert[7] = float(y);

    ret.coord[0] = 0.0;         ret.coord[1] = 0.0;
    ret.coord[2] = float(x);    ret.coord[3] = 0.0;
    ret.coord[4] = float(x);    ret.coord[5] = float(y);
    ret.coord[6] = 0.0;         ret.coord[7] = float(y);
    #else
    ret.vert[0]  = 0;       ret.vert[1]  = 0;       ret.vert[2]  = 0;
    ret.vert[3]  = int(x);  ret.vert[4]  = 0;       ret.vert[5]  = 0;
    ret.vert[6]  = 0;       ret.vert[7]  = int(y);  ret.vert[8]  = 0;
    ret.vert[9]  = int(x);  ret.vert[10] = int(y);  ret.vert[11] = 0;

    ret.coord[0] = 0.0;         ret.coord[1] = 0.0;
    ret.coord[2] = float(x);    ret.coord[3] = 0.0;
    ret.coord[4] = 0.0;         ret.coord[5] = float(y);
    ret.coord[6] = float(x);    ret.coord[7] = float(y);
    #endif
    return ret;
}

bool vbaGL::draw() {
    for (uint i = 0; i < textures.size(); i++) {
        textures[i].bind(textures[i].unit);
        if (textures[i].hasShader) {
            textures[i].prog->activate();
            textures[i].prog->setPass(i);
            textures[i].prog->setSrcTexUnit(textures[i].unit);
            if (i != textures.size() - 1) {
                textures[i].prog->setDstSz(textures[i+1].size);
            } else {
                textures[i].prog->setDstSz(vwpt_sz);
            }
            textures[i].prog->setSrcSz(textures[i].size);
            textures[i].prog->updMatrices();
        }
        if (i != textures.size() - 1) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, textures[i].size.x + 1);
            // Might end up dropping this behavior. Just experimenting for now
            /*
            vbaDrawArrs drawArrs = genDrawArrs(textures[i+1].size.x,
                                               textures[i+1].size.y);
            #ifndef VBA_TRIANGLE_STRIP
            glVertexPointer(2, GL_FLOAT, 0, drawArrs.vert);
            #else
            glVertexPointer(3, GL_INT, 0, drawArrs.vert);
            #endif
            glTexCoordPointer(2, GL_FLOAT, 0, drawArrs.coord);
            */
            textures[i+1].bindBuffer();
            glVwpt(textures[i].size);
        } else {
            /*
            #ifndef VBA_TRIANGLE_STRIP
            glVertexPointer(2, GL_FLOAT, 0, draw_vert);
            #else
            glVertexPointer(3, GL_INT, 0, draw_vert);
            #endif
            glTexCoordPointer(2, GL_FLOAT, 0, draw_coord);
            */
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
    glGenVertexArrays(1, &vtxArrVtx);
    glBindVertexArray(vtxArrVtx);
    glGenBuffers(1, &vtxBuffVtx);
    glBindBuffer(GL_ARRAY_BUFFER, vtxBuffVtx);
    #ifndef VBA_TRIANGLE_STRIP
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), draw_vert, GL_STATIC_DRAW);
    #else
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLint), draw_vert, GL_STATIC_DRAW);
    #endif

    glGenVertexArrays(1, &vtxArrTexCoord);
    glBindVertexArray(vtxArrTexCoord);
    glGenBuffers(1, &vtxBuffTexCoord);
    glBindBuffer(GL_ARRAY_BUFFER, vtxBuffTexCoord);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), draw_coord, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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
    //dummyglsl->attachShader(dummy_shd_v);
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
    //textures.emplace_back(scale, this);
    //textures.back().initBuffer();
    //textures.back().setShaderProg(dummyglsl);

    // Final texture
    textures.emplace_back(scale, this);
    textures.back().initBuffer();
    textures.back().setShaderProg(dummyglsl);

    return true;
}

/* Might do this differently */
bool vbaGL::setTexData(const GLvoid *data) {
    return textures.front().setData(data);
}
