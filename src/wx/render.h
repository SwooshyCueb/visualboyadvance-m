#ifndef VBA_RENDER_H
#define VBA_RENDER_H

#include <GL/glxew.h>
#include <GL/glut.h>
#include <wx/glcanvas.h>
#include <cstdio>
#include <cstddef>
#include <deque>
#include <queue>
#include <stack>

//#define VBA_TRIANGLE_STRIP

/* Just a struct for holding sizes or coords or what-have-you */
typedef struct vbaSize {
    GLuint x;
    GLuint y;
} vbaSize;
// TODO: Turn vbaSize into a class that can be multiplied by integers

// We need to forward declare some classes.
class glErr;
class vbaGL;
class vbaTex;

/* glErr
 * OpenGL error info object
 *
 * I know this is probably a way overcomplicated way of doing this.
 * You have my most sincere (but not really heartfelt) apologies.
 *
 * As well, I fear I may have /created/ a memory leak or two in trying to
 * avoid them. Feedback in this aspect is most certainly welcome.
 */
class glErr {
public:
    glErr(GLenum val, const char *src, uint loc, const char *fnc) {
        err = val;
        line = loc;

        file = (char*)malloc(strlen(src) + 1);
        func = (char*)malloc(strlen(fnc) + 1);

        strcpy(file, src);
        strcpy(func, fnc);

        init = true;
    }

    glErr() {
        init = false;
    }

    ~glErr() {
        if (init) {
            free(file);
            free(func);
        }
    }

    void print() {
        if (!init)
            return;
        fprintf(stderr, "%s:%u in func %s\n\tglError: %s\n", file, line, func,
               (char*)gluErrorString(err));
    }

    bool operator == (const glErr &other) {
        return (err == other.err);
    }

    bool operator == (const GLenum &other) {
        return (err == other);
    }

    glErr& operator = (const glErr &other) {
        if (init) {
            free(file);
            free(func);
        }

        err = other.err;
        line = other.line;

        file = (char*)malloc(strlen(other.file) + 1);
        func = (char*)malloc(strlen(other.func) + 1);

        strcpy(file, other.file);
        strcpy(func, other.func);

        init = true;

        return *this;
    }

    GLenum err;
    char *file;
    char *func;
    uint line;
    bool init = false;
};

// Tenative; will probably move to a different file
enum vbaErr {
    VBAERR_OK,
    VBAERR_UNKN,
    VBAERR_GLERR,
    VBAERR_GLINIT,
    VBAERR_NODATA,
    VBAERR_NOBUFF,
    VBAERR_INVALIDARGS
};

/* vbaTex
 * Texture object for use in vbaGL
 */
class vbaTex {
    friend class vbaGL;
public:
    vbaTex(uint mult, vbaGL *globj);
    ~vbaTex();

    void updSize();

    void setResizeFilter(GLint filter);
    void setOobBehavior(GLint behavior);

    bool setData(const GLvoid *data);

    bool bind();
    bool bind(uint num);

    bool initBuffer();
    bool remBuffer();

    bool bindBuffer(GLenum target);
    bool bindBuffer();
    bool bindBufferRead();
    bool bindBufferWrite();

private:
    bool glPushErr(const char *file, int line, const char *func);

    GLuint texture;
    GLuint unit;
    GLuint scale;
    GLint resizefilt;
    GLint blitmask;
    vbaSize size;

    bool hasBuffer = false;

    GLuint texbuff;
    GLuint rdrbuff;

    vbaGL *ctx;
};

/* vbaGL
 * OpenGL stuff.
 */
class vbaGL {
    friend class vbaTex;
public:
    vbaGL();
    ~vbaGL();

    void setBaseSize(uint x, uint y);
    void setVwptSize(uint x, uint y);

    bool genTextures(uint scale);
    bool setTexData(const GLvoid *data);

    bool draw();
    void clear();
    glErr glErrGet();
    void glErrPrint();

private:
    bool glPushErr(const char *file, int line, const char *func);
    #define glCheckErr() glPushErr(__FILE__, __LINE__, __func__)

    vbaSize base_sz;
    vbaSize vwpt_sz;
    std::deque<vbaTex> textures;
    GLuint largest_scale;
    std::queue<glErr> glErrs;
    std::stack<vbaErr> vbaErrs;

    #ifndef VBA_TRIANGLE_STRIP
    static GLfloat draw_vert[8];
    #else
    static GLint draw_vert[12];
    #endif
    static GLfloat draw_coord[8];
    static GLenum DrawBuffers[1];
};

#endif
