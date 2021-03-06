#ifndef VBA_TEX_H
#define VBA_TEX_H

#include "oxygen.h"
#include <GL/glxew.h>
#include <GL/glut.h>

#define DEFAULT_TEX_FMT GL_RGBA

/* vbaTex
 * Texture object for use in vbaGL
 *
 * Still needs copy constructor
 * Still needs assignment operator
 */
class vbaTex {
    friend class vbaGL;
    friend class renderStage;
public:
    vbaTex(); //dummy constructor
    vbaTex(vbaGL *globj, vbaSize sz);
    bool init(vbaGL *globj, vbaSize sz);
    vbaTex(vbaGL *globj, uint x, uint y);
    bool init(vbaGL *globj, uint x, uint y);
    vbaTex(vbaGL *globj, vbaSize sz, GLenum fmt);
    bool init(vbaGL *globj, vbaSize sz, GLenum fmt);
    vbaTex(vbaGL *globj, uint x, uint y, GLenum fmt);
    bool init(vbaGL *globj, uint x, uint y, GLenum fmt);
    ~vbaTex();

    void setSize(vbaSize sz);
    void setSize(uint x, uint y);
    vbaSize getSize();

    void setResizeFilter(GLint filter);
    void setOobBehavior(GLint behavior);

    bool setData(const GLvoid *data);

    bool bind();
    bool bind(uint num);

    // Move this back to private
    GLuint texture;

private:
    EH_DECLARE();

    GLint resizefilt;
    GLint blitmask;
    GLenum format;

    vbaSize size;

    vbaGL *ctx;

    bool is_init = false;
};

#endif
