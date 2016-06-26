#ifndef VBA_GLSL_SHADER_H
#define VBA_GLSL_SHADER_H

#include "swooshboy.h"

/* glslShader
 * Class for handling unlinked shaders
 */
class glslShader {
    friend class glslProg;
    friend class vbaGL;
public:
    /* Like a lot of classes in this project, it needs a handle for the vbaGL
     * object. As well, you must specify what kind of shader to create.
     *
     * Valid values for type_in are GL_VERTEX_SHADER and GL_FRAGMENT_SHADER
     */
    glslShader(vbaGL *globj, GLenum type_in);

    /* The source file object is not specified in the constructor
     * because reasons. Do it here instead.
     */
    bool setSrc(glslSrc *srcobj);

    // Attempts to compile the shader
    bool compile();
    // Prints the compile log to stderr
    bool printInfoLog();

protected:
    EH_DECLARE();

    GLuint shader;

    GLint compiled;
    GLenum type;

    glslSrc *src;
    GLchar *glsl[5];
    GLint glsl_len[5];
    vbaGL *ctx;

    // Our preprocessor directive strings are declared here
    static GLchar *glsl_version;
    static GLchar *glsl_defines_global;
    static GLchar *glsl_defines_vert;
    static GLchar *glsl_defines_frag;
};

#endif
