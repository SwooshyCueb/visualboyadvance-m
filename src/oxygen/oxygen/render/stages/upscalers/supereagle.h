#ifndef VBA_STG_SUPEREAGLE_H
#define VBA_STG_SUPEREAGLE_H

#include "render/pipeline.h"
#include "oxygen.h"

class stgSuperEagle : public renderStage {
public:
    stgSuperEagle(vbaGL *globj);
    bool init(vbaGL *globj);
    ~stgSuperEagle();

    bool setIndex(uint idx, renderPipeline *rdrpth);
    bool render(vbaTex *src);

private:
    struct {
        GLint texcoord = 0;
        GLint position = 0;
        GLint needs_flip = 0;
        GLint v_dst_sz = 0;
        GLint f_dst_sz = 0;
        GLint v_src_sz = 0;
        GLint f_src_sz = 0;
        GLint src_tex;
    } glsl_vars;
};

#endif
