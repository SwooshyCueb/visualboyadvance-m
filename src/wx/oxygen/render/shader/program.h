#ifndef VBA_GLSL_PROGRAM_H
#define VBA_GLSL_PROGRAM_H

#include "oxygen.h"
#include "render/size.h"
#include "sharedvar.h"

/* sharedGlProg
 * Comtainer for OpenGL program objects
 * Ensures shader programs are not deinitialized while still in use
 */
class sharedGlProg : public vbaSharedObj {
    friend class glslProg;
public:
    sharedGlProg();
    ~sharedGlProg();

private:
    bool attachShader(glslShader *shader);
    bool detachShaders();

    bool link();

    GLuint program;
    glslShader *v;
    glslShader *f;

    GLint linked = 0;
    bool has_vtx = false;
    bool has_frag = false;
};

/* glslProg
 * Class for handling glsl programs
 *
 * Still needs copy constructor
 * Still needs assignment operator
 */
class glslProg {
    friend class vbaGL;
    friend class glslShader;
    friend class stgOSD;
public:
    glslProg(); // dummy constructor
    glslProg(vbaGL *globj);
    bool init(vbaGL *globj);
    ~glslProg();

    bool printInfoLog();
    bool attachShader(glslShader *shader);
    bool detachShaders();

    bool link();
    bool activate();

    GLint getUniformPtr(const char *name);
    void setVar1i(GLint var, GLint val);
    void setVar2i(GLint var, GLint val1, GLint val2);
    void setVar1f(GLint var, GLfloat val);
    void setVar2f(GLint var, GLfloat val1, GLfloat val2);
    void setVar4f(GLint var, GLfloat val1, GLfloat val2, GLfloat val3,
                  GLfloat val4);

    GLint getAttrPtr(const char *name);
    bool enableVertAttrArr(const GLint arr);
    bool disableVertAttrArr(const GLint arr);
    bool setVtxAttrPtr(const GLint arr, GLint sz, GLenum typ, GLboolean norm,
                       GLsizei stride, const GLvoid *ptr);

    // same program, same shaders
    bool shallowCopy(const glslProg &other);
    // new program, same shaders
    bool deepCopy(const glslProg &other);
    // If you need a new program with new shaders, just make a new glslProg

    glslProg(const glslProg &other);
    glslProg &operator  = (const glslProg &other);
    // TODO: (in)equality operator?

private:
    EH_DECLARE();

    sharedGlProg *program;

    bool is_init = false;
    bool deinit();

    vbaGL *ctx;
};

#endif

