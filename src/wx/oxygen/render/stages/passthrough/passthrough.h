#ifndef VBA_STG_PASSTHROUGH_H
#define VBA_STG_PASSTHROUGH_H

#include "render/stage.h"
#include "oxygen.h"

class stgPassthrough : public renderStage {
public:
    stgPassthrough(vbaGL *globj);
    bool init(vbaGL *globj);
    ~stgPassthrough();

    bool setIndex(uint idx, renderPipeline *rdrpth);
    bool render(vbaTex *src);

private:
    struct {
        GLint texcoord = 0;
        GLint position = 0;
        GLint needs_flip = 0;
        GLint src_tex = 0;
    } glsl_vars;
};

#endif
