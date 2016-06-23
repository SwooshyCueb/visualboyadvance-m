#ifndef VBA_SHADERS_H
#define VBA_SHADERS_H
#include "swooshboy.h"
#include "glsl.h"
#include "vbaerr.h"

class glslCommonSrc : public glslSrc {
public:
    glslCommonSrc();
};

class shdPassthrough : public glslProg {
public:
    shdPassthrough(vbaGL *globj);
    bool init();
};

class shdSuperEagle : public glslProg {
public:
    shdSuperEagle(vbaGL *globj);
    bool init();
};

#endif
