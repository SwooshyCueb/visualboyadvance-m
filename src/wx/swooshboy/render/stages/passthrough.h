#ifndef VBA_STG_PASSTHROUGH_H
#define VBA_STG_PASSTHROUGH_H

#include "render/pipeline.h"
#include "swooshboy.h"

class stgPassthrough : public renderStage {
public:
    stgPassthrough(vbaGL *globj);
    bool init(vbaGL *globj);
    bool setIndex(uint idx);
    bool render(vbaTex *src);
};

#endif
