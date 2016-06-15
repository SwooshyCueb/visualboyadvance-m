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

    /* Standard constructor
     *
     * Takes a GL error enum, the filename of the source file the error came
     * from, the line number where the error takes place, and the function in
     * which it occurred.
     */
    glErr(GLenum val, const char *src, uint loc, const char *fnc);

    /* Dummy constructor
     *
     * This is pretty much just here to make life easier for me.
     */
    glErr();

    /* Destructor
     *
     * Does what it says on the tin.
     */
    ~glErr();


    /* Error printing function
     *
     * Prints information about the GL error to stderr
     */
    void print();

    /* = operator
     *
     * Does what it says on the tin
     */
    glErr& operator = (const glErr &other);


    /* Most of the stuff below here should probably be private, but I'm not
     * bothering with that for this particular class. At least not at the
     * moment.
     */

    GLenum err; //error constant
    char *file; //filename
    char *func; //function name
    uint line; //line number
    bool init = false; //is the instance properly initialized?
};

#endif
