#include "oxygen.h"
#include "src.h"

glslSrc::glslSrc() {
    is_init = false;
}

glslSrc::glslSrc(char *src_in, const uint len) {
    init(src_in, len);
}

bool glslSrc::init(char *src_in, const uint len) {
    src_main = src_in;
    src_main_len = len;
    is_init = true;

    return true;
}

glslSrc::~glslSrc() {
    // Nothing to do here
}

glslSrc::glslSrc(const glslSrc &other) {
    src_main = other.src_main;
    src_main_len = other.src_main_len;
}

glslSrc &glslSrc::operator = (const glslSrc &other) {
    src_main = other.src_main;
    src_main_len = other.src_main_len;
    return *this;
}

DECLARE_RES(glsl_common);

glslCommonSrc::glslCommonSrc()
    : glslSrc(res_glsl_common, res_glsl_common_len) {
}

glslCommonSrc::~glslCommonSrc() {
    // Nothing to do here
}
