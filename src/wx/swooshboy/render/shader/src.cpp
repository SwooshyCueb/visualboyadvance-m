#include "swooshboy.h"
#include "src.h"

glslSrc::glslSrc(char *src_in, const uint len) {
    src_main = src_in;
    src_main_len = len;
}

DECLARE_RES(glsl_common);
glslCommonSrc::glslCommonSrc()
    : glslSrc(res_glsl_common, res_glsl_common_len) {
}
