#ifndef VBA_GL_H
#define VBA_GL_H

#include "oxygen.h"
#include "pipeline.h"
#include <GL/glxew.h>
#include <GL/glut.h>
#include <deque>
#include <queue>
#include <stack>

// There's no reason to pretend I'm targeting anything but linux at this point
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>


// Moving these here for my reference
/*
#define int_fmt out_16 ? GL_RGB5 : GL_RGB
#define tex_fmt out_16 ? GL_BGRA : GL_RGBA, \
                out_16 ? GL_UNSIGNED_SHORT_1_5_5_5_REV : GL_UNSIGNED_BYTE
*/

/* vbaGL
 * OpenGL stuff.
 *
 * Still needs copy constructor
 * Still needs assignment operator
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
    bool setVsyncState(int vsync);

    vbaSize getBaseSize();
    vbaSize getVwptSize();
    float getBaseScale();

    bool initPipeline(uint scale);
    bool render(const GLvoid *data);

    bool glVwpt(uint x, uint y);
    bool glVwpt(vbaSize sz);

    void clear();
    vbaErr errGet();
    bool errPrint();

    // Provide getters for these and move back to private
    GLuint vb_vtx;
    GLuint vb_texcoord;
    static GLenum DrawBuffers[1];

private:
    bool draw();

    renderPipeline pipeline;

    EH_DECLARE();

    vbaSize base_sz;
    vbaSize vwpt_sz;
    float base_scale;

    GLuint largest_scale;
    std::queue<vbaErr> vbaErrs;

    #ifndef VBA_TRIANGLE_STRIP
    static GLfloat draw_vert[8];
    #else
    static GLint draw_vert[12];
    #endif
    static GLfloat draw_coord[8];

    GLuint vtxArr;

    glslCommonSrc glsl_common;

    bool init_p = false;
};

#endif
