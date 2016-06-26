#ifndef VBA_STG_SUPEREAGLE_H
#define VBA_STG_SUPEREAGLE_H

#include "render/pipeline.h"
#include "swooshboy.h"

class stgSuperEagle : public renderStage {
public:
    stgSuperEagle(vbaGL *globj);
    bool init(vbaGL *globj);
    bool setIndex(uint idx, renderPipeline *rdrpth);
    bool render(vbaTex *src);
};

#endif
