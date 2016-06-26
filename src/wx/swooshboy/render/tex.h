#ifndef VBA_TEX_H
#define VBA_TEX_H

#include "swooshboy.h"
#include <GL/glxew.h>
#include <GL/glut.h>

/* vbaTex
 * Texture object for use in vbaGL
 *
 * Destructor needs work
 * Still needs copy constructor
 * Still needs assignment operator
 */
class vbaTex {
    friend class vbaGL;
    friend class renderStage;
public:
    vbaTex(); //dummy constructor
    //TODO: Allow texture creation with explicit width and height
    vbaTex(float scale, vbaGL *globj);
    bool init(float scale, vbaGL *globj);
    ~vbaTex();

    void updSize(float scale);

    void setResizeFilter(GLint filter);
    void setOobBehavior(GLint behavior);
    void setShaderProg(glslProg *program);

    bool setData(const GLvoid *data);

    bool bind();
    bool bind(uint num);

    // move back to private and create getter and setter
    GLuint scale;
    vbaSize size;

private:
    EH_DECLARE();

    GLuint texture;
    GLint resizefilt;
    GLint blitmask;

    vbaGL *ctx;

    bool is_init = false;
};

#endif
