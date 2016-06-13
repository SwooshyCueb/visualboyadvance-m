#include "swooshboy.h"
#include "glerr.h"
#include <cstdio>
#include <cstring>

glErr::glErr(GLenum val, const char *src, uint loc, const char *fnc) {
    err = val;
    line = loc;

    file = (char*)malloc(strlen(src) + 1);
    func = (char*)malloc(strlen(fnc) + 1);

    strcpy(file, src);
    strcpy(func, fnc);

    init = true;
}

glErr::glErr() {
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
    fprintf(stderr, "%s:%u in func %s\n\tglError: %s\n", file, line, func,
           (char*)gluErrorString(err));
}

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

glErr &glErr::operator = (const glErr &other) {
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
