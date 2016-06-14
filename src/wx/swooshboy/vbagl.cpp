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
    glEnable(GL_TEXTURE_2D);
    //glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_DEPTH_TEST);
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

void vbaGL::glErrPrint() {
    uint errs = glErrs.size();
    for (uint i = 0; i < errs; i++) {
        glErrs.front().print();
        glErrs.pop();
    }
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
        //textures[i].bind();
        textures[i].bind(textures[i].unit);
        if (i != textures.size() - 1) {
            //textures[i].bind(textures[i].unit);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, textures[i].size.x + 1);
            //glDisable(GL_CULL_FACE);
            //glDisable(GL_DEPTH_TEST);
            //glEnable(GL_CULL_FACE);
            //glEnable(GL_DEPTH_TEST);
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
            glVwpt(textures[i+1].size);
            //glVwpt(1, 1);
        } else {
            //textures[i].bind(0);
            //glDisable(GL_CULL_FACE);
            //glDisable(GL_DEPTH_TEST);
            //glEnable(GL_CULL_FACE);
            //glEnable(GL_DEPTH_TEST);
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
    }
    return !glCheckErr();
}

void vbaGL::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
}

bool vbaGL::initShaders() {
    glslSrc dummyfrag_src;
    LOAD_GLSL_SRC(dummyfrag_src, dummy_fragment);
    glslShader dummyfrag_shd(this, GL_FRAGMENT_SHADER);
    dummyfrag_shd.setSrc(&dummyfrag_src);
    dummyfrag_shd.compile();
    return true;
}

/* Might do this differently */
bool vbaGL::genTextures(uint scale) {
    textures.emplace_back(scale, this);
    textures.emplace_back(scale, this);
    textures.back().initBuffer();
    textures.emplace_back(0, this);
    textures.back().initBuffer();
    return true;
}

/* Might do this differently */
bool vbaGL::setTexData(const GLvoid *data) {
    return textures.front().setData(data);
}
