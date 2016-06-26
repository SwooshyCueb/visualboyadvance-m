#ifndef VBA_GL_H
#define VBA_GL_H

#include "swooshboy.h"
#include "pipeline.h"
#include <GL/glxew.h>
#include <GL/glut.h>
#include <deque>
#include <queue>
#include <stack>


// Moving these here for my reference
/*
#define int_fmt out_16 ? GL_RGB5 : GL_RGB
#define tex_fmt out_16 ? GL_BGRA : GL_RGBA, \
                out_16 ? GL_UNSIGNED_SHORT_1_5_5_5_REV : GL_UNSIGNED_BYTE
*/

/* vbaGL
 * OpenGL stuff.
 */
class vbaGL {
    friend class vbaTex;
    friend class glslSrc;
    friend class glslShader;
    friend class glslProg;
    friend class vbaOSD;
    friend class renderPipeline;
    friend class renderStage;
public:
    vbaGL();
    ~vbaGL();

    void setBaseSize(uint x, uint y);
    void setVwptSize(uint x, uint y);
    void setBaseScale(float scale);

    bool initPipeline(uint scale);
    bool render(const GLvoid *data);
    bool setVsyncState(int vsync);

    bool glVwpt(uint x, uint y);
    bool glVwpt(vbaSize sz);

    void clear();
    vbaErr errGet();
    bool errPrint();

    // Move these back to private and create some getters
    vbaSize base_sz;
    vbaSize vwpt_sz;
    float base_scale;

    // This needs to be private
    renderPipeline *pipeline;

private:
    bool draw();

    EH_DECLARE();

    GLuint largest_scale;
    std::queue<vbaErr> vbaErrs;

    #ifndef VBA_TRIANGLE_STRIP
    static GLfloat draw_vert[8];
    #else
    static GLint draw_vert[12];
    #endif
    static GLfloat draw_coord[8];
    static GLenum DrawBuffers[1];

    GLuint vtxArr;
    GLuint vb_vtx;
    GLuint vb_texcoord;

    glslCommonSrc glsl_common;

    bool init_p = false;
};

#endif
