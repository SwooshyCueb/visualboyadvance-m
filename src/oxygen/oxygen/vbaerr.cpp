#include "oxygen.h"
#include "vbaerr.h"
#include <cstdio>
#include <cstring>

vbaErr::vbaErr(vbaErrVal val, const char *src, uint loc, const char *fnc) {
    set(val, GL_NO_ERROR, FT_Err_Ok, src, loc, fnc);
}

vbaErr::vbaErr(vbaErrVal val, GLenum glval, const char *src, uint loc,
               const char *fnc) {
    set(val, glval, FT_Err_Ok, src, loc, fnc);
}

vbaErr::vbaErr(vbaErrVal val, FT_Error ftval, const char *src, uint loc,
               const char *fnc) {
    set(val, GL_NO_ERROR, ftval, src, loc, fnc);
}

vbaErr::vbaErr(vbaErrVal val, GLenum glval, FT_Error ftval, const char *src, uint loc,
               const char *fnc) {
    set(val, glval, ftval, src, loc, fnc);
}

void vbaErr::set(vbaErrVal val, const char *src, uint loc, const char *fnc) {
    set(val, GL_NO_ERROR, FT_Err_Ok, src, loc, fnc);
}

void vbaErr::set(vbaErrVal val, FT_Error ftval, const char *src, uint loc,
                 const char *fnc) {
    set(val, GL_NO_ERROR, ftval, src, loc, fnc);
}

void vbaErr::set(vbaErrVal val, GLenum glval, const char *src, uint loc,
                 const char *fnc) {
    set(val, glval, FT_Err_Ok, src, loc, fnc);
}

void vbaErr::set(vbaErrVal val, GLenum glval, FT_Error ftval,
                 const char *src, uint loc, const char *fnc) {
    // If we are already initialized, free those strings!
    if (is_init) {
        free(file);
        free(func);
    }

    err = val;
    glErr = glval;
    ftErr = ftval;
    line = loc;

    file = (char*)malloc(strlen(src) + 1);
    func = (char*)malloc(strlen(fnc) + 1);

    // If the compiler doesn't end the strings with /0, this won't work
    strcpy(file, src);
    strcpy(func, fnc);

    is_init = true;
}

vbaErr::vbaErr() {
    // Dummy constructor
    is_init = false;
}

vbaErr::~vbaErr() {
    if (is_init) {
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

const char* vbaErr::ftGetStr(FT_Error val) {
    #undef __FTERRORS_H__
    #define FT_ERRORDEF(code, value, string) case code: return string;
    #define FT_ERROR_START_LIST switch (val) {
    #define FT_ERROR_END_LIST }
    return "Unknown error";
}

void vbaErr::print() {
    if (!is_init)
        return;
    // Should probably use dprintf here
    fprintf(stderr, "%s:%u in func %s: error: %s\n", file, line, func,
            vbaGetStr(err));
    if (glErr != GL_NO_ERROR)
        fprintf(stderr, "\tGL error: %s\n", (char*)gluErrorString(glErr));
    if (ftErr != FT_Err_Ok)
        fprintf(stderr, "\tFreeType error: %s\n", ftGetStr(ftErr));
}

/* When checking equality between objects, just compare the error values */
inline bool operator == (const vbaErr &left, const vbaErr &right) {
    return ((left.err == right.err) &&
            (left.glErr == right.glErr) &&
            (left.ftErr == right.ftErr));
}
inline bool operator != (const vbaErr &left, const vbaErr &right) {
    return ((left.err != right.err) ||
            (left.glErr != right.glErr) ||
            (left.ftErr != right.ftErr));
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

/* when comparing to a GL error constant, treat the object as another GLenum */
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

/* when comparing to an FT_Error, treat the object as another FT_Error */
inline bool operator == (const vbaErr &obj, const FT_Error &num) {
    return (obj.ftErr == num);
}
inline bool operator != (const vbaErr &obj, const FT_Error &num) {
    return (obj.ftErr != num);
}
inline bool operator == (const FT_Error &num, const vbaErr &obj) {
    return (obj.ftErr == num);
}
inline bool operator != (const FT_Error &num, const vbaErr &obj) {
    return (obj.ftErr != num);
}

vbaErr &vbaErr::operator = (const vbaErr &other) {
    set(other.err, other.glErr, other.ftErr, other.file, other.line,
        other.func);
    return *this;
}
