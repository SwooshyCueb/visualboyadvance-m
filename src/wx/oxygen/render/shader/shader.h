#ifndef VBA_GLSL_SHADER_H
#define VBA_GLSL_SHADER_H

#include "oxygen.h"

/* glslShader
 * Class for handling individual shaders
 */
class glslShader {
    friend class glslProg;
    friend class sharedGlProg;
    friend class vbaGL;
public:
    glslShader(); // dummy constructor
    /* Like a lot of classes in this project, it needs a handle for the vbaGL
     * object. As well, you must specify what kind of shader to create.
     *
     * Valid values for type_in are GL_VERTEX_SHADER and GL_FRAGMENT_SHADER
     */
    glslShader(vbaGL *globj, GLenum type_in);
    bool init(vbaGL *globj, GLenum type_in);
    ~glslShader();

    /* The source file object is not specified in the constructor
     * because reasons. Do it here instead.
     */
    bool setSrc(glslSrc *srcobj);

    // Attempts to compile the shader
    bool compile();
    // Prints the compile log to stderr
    bool printInfoLog();

    /* Copies another glslShader object into this one.
     * Creates a NEW glShader.
     */
    bool copy(const glslShader &other);

    glslShader(const glslShader &other);
    glslShader &operator  = (const glslShader &other);
    // TODO: (in)equality operator?

private:
    EH_DECLARE();

    bool is_init = false;
    bool deinit();

    GLuint shader;

    GLint compiled = 0;
    GLenum type;

    glslSrc *src;
    GLchar *glsl[5];
    GLint glsl_len[5];
    bool src_set = false;

    vbaGL *ctx;

    void commonCopy(const glslShader &other);

    // Our preprocessor directive strings are declared here
    static GLchar *glsl_version;
    static GLchar *glsl_defines_global;
    static GLchar *glsl_defines_vert;
    static GLchar *glsl_defines_frag;
};

#endif
