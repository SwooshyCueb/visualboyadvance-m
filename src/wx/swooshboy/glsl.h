#ifndef VBA_GLSL_H
#define VBA_GLSL_H
#include "swooshboy.h"
#include "size.h"
#include "swooshboy/glsl/dummy.glsl.h"

/* glslSrc
 * glsl source container object
 *
 * glsl source files are included in VBA as const unsigned char[] variables
 * generated by cmake. This class just makes handling them a little easier.
 *
 * This seemed like the most sensible way to do this, as the same glsl source
 * file can contain multiple glsl shaders, and can be reused. Thanks to the way
 * glShaderSource() handles these variables, we can prepend preprocessor
 * directives without modifying the glsl source variable.
 */
class glslSrc {
    friend class glslShader;
    friend class vbaGL;
public:
    // Constructor doens't do much of note
    glslSrc();

    // This function takes the two variables generated by cmake
    void loadSrc(unsigned char *src_in, const uint len);
    // This macro makes using the above function a little easier
    #define LOAD_GLSL_SRC(obj, name) \
        obj.loadSrc(const_cast<unsigned char *>(name##_glsl), name##_glsl_len)

private:
    // The glsl source
    char *src_main;
    // Length of the array
    uint src_main_len;
};

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

private:
    // The glErr functions call their counterparts in the vbaGL object
    bool glPushErr(const char *file, int line, const char *func);
    #ifndef glCheckErr
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)
    #endif
    bool glPushErr(const char *file, int line, const char *func, GLenum err);
    #ifndef glIgnoreErr
    #define glIgnoreErr(err) glPushErr(__FILE__, __LINE__, __func__, err);
    #endif

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

/* glslProg
 * Class for handling glsl programs
 */
class glslProg {
    friend class vbaGL;
public:
    glslProg(vbaGL *globj);

    bool printInfoLog();
    bool attachShader(glslShader shader);

    bool init();
    bool activate();

    void setPassQty(uint n);
    void setPassIdx(uint n);
    void setSrcTexUnit(GLuint n);
    void setSrcSz(vbaSize sz);
    void setDstSz(vbaSize sz);
    void setNeedsFlip(bool flip);

private:
    bool glPushErr(const char *file, int line, const char *func);
    #ifndef glCheckErr
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)
    #endif
    bool glPushErr(const char *file, int line, const char *func, GLenum err);
    #ifndef glIgnoreErr
    #define glIgnoreErr(err) glPushErr(__FILE__, __LINE__, __func__, err);
    #endif

    GLint getUniformPtr(const char *name);
    void setVar1i(GLint var, GLint val);
    void setVar2f(GLint var, GLfloat val1, GLfloat val2);

    GLint getAttrPtr(const char *name);
    bool enableVertAttrArr(const GLint arr);
    bool disableVertAttrArr(const GLint arr);
    bool setVtxAttrPtr(const GLint arr, GLint sz, GLenum typ, GLboolean norm,
                       GLsizei stride, const GLvoid *ptr);

    void updMatrices();

    GLuint program;
    GLint linked;
    bool hasVtx = false;
    bool hasFrag = false;

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
