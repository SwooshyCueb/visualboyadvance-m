#include "swooshboy.h"
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

DECLARE_RES(glsl_common);
glslCommonSrc::glslCommonSrc()
    : glslSrc(res_glsl_common, res_glsl_common_len) {
}
