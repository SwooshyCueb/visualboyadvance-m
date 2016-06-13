#ifndef VBA_GLERR_H
#define VBA_GLERR_H

#include "swooshboy.h"
#include <GL/glxew.h>
#include <GL/glut.h>

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
    glErr(GLenum val, const char *src, uint loc, const char *fnc);
    glErr();
    ~glErr();

    void print();
    glErr& operator = (const glErr &other);

    GLenum err;
    char *file;
    char *func;
    uint line;
    bool init = false;
};

#endif
