#ifndef VBA_STG_SUPEREAGLE_H
#define VBA_STG_SUPEREAGLE_H

#include "render/pipeline.h"
#include "swooshboy.h"

class stgSuperEagle : public renderStage {
public:
    stgSuperEagle(vbaGL *globj);
    bool setIndex(uint idx);
    bool render(vbaTex *src);
};

#endif
