#include "swooshboy.h"
#include "shaders.h"
#include "glsl.h"
#include "vbaerr.h"

/* Moving to a different way of handling shaders.
 * glsl.cpp and glsl.h will still contain the core functionality, but each
 * shader shall be its own subclass of glslProg.
 */

DECLARE_RES(glsl_common);

glslCommonSrc::glslCommonSrc()
    : glslSrc(res_glsl_common, res_glsl_common_len) {
}

shdPassthrough::shdPassthrough(vbaGL *globj)
    : glslProg(globj) {

    CREATE_GLSL_SRC_OBJ(pt_src, passthrough);

    glslShader shd_f(globj, GL_FRAGMENT_SHADER);
    glslShader shd_v(globj, GL_VERTEX_SHADER);
    shd_f.setSrc(&pt_src);
    shd_f.compile();
    shd_v.setSrc(&pt_src);
    shd_v.compile();

    glslProg::attachShader(shd_f);
    glslProg::attachShader(shd_v);
    glslProg::init();

}

bool shdPassthrough::init() {
    return glslProg::init();
}

shdSuperEagle::shdSuperEagle(vbaGL *globj)
    : glslProg(globj) {

    CREATE_GLSL_SRC_OBJ(se_src, supereagle);

    glslShader shd_f(globj, GL_FRAGMENT_SHADER);
    glslShader shd_v(globj, GL_VERTEX_SHADER);
    shd_f.setSrc(&se_src);
    shd_f.compile();
    shd_v.setSrc(&se_src);
    shd_v.compile();

    glslProg::attachShader(shd_f);
    glslProg::attachShader(shd_v);
    glslProg::init();
}

bool shdSuperEagle::init() {
    return glslProg::init();
}
