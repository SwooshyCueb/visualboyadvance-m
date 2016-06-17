#include "swooshboy.h"
#include "vbaerr.h"
#include <cstdio>
#include <cstring>

vbaErr::vbaErr(vbaErrVal val, const char *src, uint loc, const char *fnc) {
    set(val, GL_NO_ERROR, src, loc, fnc);
}

vbaErr::vbaErr(vbaErrVal val, GLenum glval, const char *src, uint loc,
               const char *fnc) {
    set(val, glval, src, loc, fnc);
}

void vbaErr::set(vbaErrVal val, const char *src, uint loc, const char *fnc) {
    set(val, GL_NO_ERROR, src, loc, fnc);
}

void vbaErr::set(vbaErrVal val, GLenum glval,
                 const char *src, uint loc, const char *fnc) {
    // If we are already initialized, free those strings!
    if (init) {
        free(file);
        free(func);
    }

    err = val;
    glErr = glval;
    line = loc;

    file = (char*)malloc(strlen(src) + 1);
    func = (char*)malloc(strlen(fnc) + 1);

    // If the compiler doesn't end the strings with /0, this won't work
    strcpy(file, src);
    strcpy(func, fnc);

    init = true;
}

vbaErr::vbaErr() {
    // Dummy constructor
    init = false;
}

vbaErr::~vbaErr() {
    if (init) {
        free(file);
        free(func);
    }
}

const char* vbaErr::vbaGetStr(vbaErrVal val) {
    #undef VBA_ERR_H
    #define VBAERR_DEF(code, string, value) case code: return string;
    #define VBAERR_START_LIST switch (val) {
    #define VBAERR_END_LIST }
    return "Unknown error";
}

void vbaErr::print() {
    if (!init)
        return;
    // Should probably use dprintf here
    fprintf(stderr, "%s:%u in func %s: error: %s\n", file, line, func,
            vbaGetStr(err));
    if (glErr != GL_NO_ERROR)
        fprintf(stderr, "\tGL error: %s\n", (char*)gluErrorString(err));
}

/* When checking equality between objects, just compare the error values */
inline bool operator == (const vbaErr &left, const vbaErr &right) {
    return ((left.err == right.err) && (left.glErr == right.glErr));
}
inline bool operator != (const vbaErr &left, const vbaErr &right) {
    return ((left.err != right.err) || (left.glErr != right.glErr));
}

/* when comparing to a vbaErrVal, treat the object as another vbaErrVal */
inline bool operator == (const vbaErr &obj, const vbaErrVal &num) {
    return (obj.err == num);
}
inline bool operator != (const vbaErr &obj, const vbaErrVal &num) {
    return (obj.err != num);
}
inline bool operator == (const vbaErrVal &num, const vbaErr &obj) {
    return (obj.err == num);
}
inline bool operator != (const vbaErrVal &num, const vbaErr &obj) {
    return (obj.err != num);
}

/* when comparing to a GLenum error constant, treat the object as another GLenum
 */
inline bool operator == (const vbaErr &obj, const GLenum &num) {
    return (obj.glErr == num);
}
inline bool operator != (const vbaErr &obj, const GLenum &num) {
    return (obj.glErr != num);
}
inline bool operator == (const GLenum &num, const vbaErr &obj) {
    return (obj.glErr == num);
}
inline bool operator != (const GLenum &num, const vbaErr &obj) {
    return (obj.glErr != num);
}

vbaErr &vbaErr::operator = (const vbaErr &other) {
    set(other.err, other.glErr, other.file, other.line, other.func);
    return *this;
}
