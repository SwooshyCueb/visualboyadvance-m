#include "swooshboy.h"
#include "glerr.h"
#include <cstdio>
#include <cstring>

glErr::glErr(GLenum val, const char *src, uint loc, const char *fnc) {
    err = val;
    line = loc;

    file = (char*)malloc(strlen(src) + 1);
    func = (char*)malloc(strlen(fnc) + 1);

    // If the compiler doesn't end the strings with /0, this won't work
    strcpy(file, src);
    strcpy(func, fnc);

    init = true;
}

glErr::glErr() {
    // Dummy constructor
    init = false;
}

glErr::~glErr() {
    if (init) {
        free(file);
        free(func);
    }
}

void glErr::print() {
    if (!init)
        return;
    // Should probably use dprintf here
    fprintf(stderr, "%s:%u in func %s\n\tglError: %s\n", file, line, func,
           (char*)gluErrorString(err));
}

/* When checking equality, this class should be treated as a GLenum. */
inline bool operator == (const glErr &left, const glErr &right) {
    return (left.err == right.err);
}
inline bool operator != (const glErr &left, const glErr &right) {
    return (left.err != right.err);
}

inline bool operator == (const glErr &obj, const GLenum &num) {
    return (obj.err == num);
}
inline bool operator != (const glErr &obj, const GLenum &num) {
    return (obj.err != num);
}
inline bool operator == (const GLenum &num, const glErr &obj) {
    return (obj.err == num);
}
inline bool operator != (const GLenum &num, const glErr &obj) {
    return (obj.err != num);
}

/* At the moment, this is the only way to actually do anything with
 * instances constructed with the dummy constructor. I should probably
 * add a function to manually initialize the instance, but eh
 */
glErr &glErr::operator = (const glErr &other) {

    // If we /are/ already initialized, free those strings!
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
