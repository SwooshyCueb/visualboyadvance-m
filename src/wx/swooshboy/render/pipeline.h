#ifndef VBA_PIPELINE_H
#define VBA_PIPELINE_H
#include "swooshboy.h"
#include "stage.h"
#include "vbaerr.h"
#include "shader/glsl.h"
#include "tex.h"

/*
 * Destructor needs work
 * Still needs copy constructor
 * Still needs assignment operator
 */
class renderPipeline {
    friend class vbaGL;
    friend class renderStage;
public:
    renderPipeline(); //dummy constructor
    renderPipeline(vbaGL *globj);
    bool init(vbaGL *globj);
    ~renderPipeline();

    bool addStage(renderStage *stg);
    bool removeStage(uint idx);
    bool refreshStages();
    uint getSize();
    bool render(const void* data);
    bool draw();

private:

    renderStage *stageAt(uint idx);


    EH_DECLARE();

    vbaTex base;
    // TODO: store actual renderStage objects here instead of pointers
    std::deque<renderStage *> pipeline;

    glslProg shd_draw; //Should be passthrough shader

    vbaGL *ctx;

    bool is_init = false;
};


#endif
