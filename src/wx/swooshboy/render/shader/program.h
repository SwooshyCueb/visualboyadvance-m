#ifndef VBA_GLSL_PROGRAM_H
#define VBA_GLSL_PROGRAM_H

#include "swooshboy.h"
#include "render/size.h"

/* glslProg
 * Class for handling glsl programs
 *
 * Still needs copy constructor
 * Still needs assignment operator
 */
class glslProg {
    friend class vbaGL;
    friend class glslShader;
public:
    glslProg(); // dummy constructor
    glslProg(vbaGL *globj);
    bool init(vbaGL *globj);
    ~glslProg();

    bool printInfoLog();
    bool attachShader(glslShader *shader);

    bool link();
    bool activate();

    void setPassQty(uint n);
    void setPassIdx(uint n);
    void setSrcTexUnit(GLuint n);
    void setSrcSz(vbaSize sz);
    void setDstSz(vbaSize sz);
    void setNeedsFlip(bool flip);

private:
    EH_DECLARE();

    GLint getUniformPtr(const char *name);
    void setVar1i(GLint var, GLint val);
    void setVar2i(GLint var, GLint val1, GLint val2);
    void setVar2f(GLint var, GLfloat val1, GLfloat val2);

    GLint getAttrPtr(const char *name);
    bool enableVertAttrArr(const GLint arr);
    bool disableVertAttrArr(const GLint arr);
    bool setVtxAttrPtr(const GLint arr, GLint sz, GLenum typ, GLboolean norm,
                       GLsizei stride, const GLvoid *ptr);

    GLuint program;
    GLint linked;
    bool has_vtx = false;
    bool has_frag = false;

    bool is_init = false;

    vbaGL *ctx;

    glslShader *v;
    glslShader *f;

    struct {
        struct {
            GLint position = 0;
            GLint texcoord = 0;

            GLint src_sz = 0;
            GLint dst_sz = 0;

            GLint pass_idx = 0;
            GLint pass_qty = 0;
        } v;

        struct {
            GLint src_tex = 0;

            GLint src_sz = 0;
            GLint dst_sz = 0;

            GLint pass_idx = 0;
            GLint pass_qty = 0;
        } f;

        GLint needs_flip = 0; // Can be in either, but not both

    } vars;
};

#endif

