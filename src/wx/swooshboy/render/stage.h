#ifndef VBA_STAGE_H
#define VBA_STAGE_H
#include "swooshboy.h"
#include "pipeline.h"
#include "shader/glsl.h"
#include "tex.h"

/*
 * Still needs destructor
 * Still needs copy constructor
 * Still needs assignment operator
 */
class renderStage {
    friend class vbaGL;
    friend class renderPipeline;
public:
    renderStage(); //dummy constructor
    renderStage(vbaGL *globj);
    bool init(vbaGL *globj);

    vbaSize getSize();

protected:
    virtual bool setIndex(uint idx, renderPipeline *rdrpth);
    virtual bool render(vbaTex *src);
    renderStage *stageAt(uint idx);

    bool setMult(uint coeff);

    uint index;
    vbaTex texture; //Texutre we are rendering TO
    glslProg shader;
    GLuint buffer;

    vbaGL *ctx;
    renderPipeline *pipeline;

    bool init_t = false;
    bool init_s = false;
    bool init_b = false;

    bool is_init = false;
private:
    bool init_m = false;
    EH_DECLARE();

    float mult; // Scale as compared to previous stage
    float scale; // Scale as compared to first stage
};

#endif
