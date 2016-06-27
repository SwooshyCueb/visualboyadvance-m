#ifndef VBA_STAGE_H
#define VBA_STAGE_H
#include "swooshboy.h"
#include "pipeline.h"
#include "shader/glsl.h"
#include "tex.h"

/*
 * Still needs copy constructor
 * Still needs assignment operator
 *
 * Maybe implement separate functions for initializing/deinitializing texture,
 * shader, and buffer?
 *
 * Maybe implement a more elegant method of updating internal information than
 * just calling setIndex again?
 */
class renderStage {
    friend class vbaGL;
    friend class renderPipeline;
public:
    renderStage(); //dummy constructor
    renderStage(vbaGL *globj);
    bool init(vbaGL *globj);
    virtual ~renderStage();

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

    bool has_texture = false;
    bool has_shader = false;
    bool has_buffer = false;

    bool is_init = false;
private:
    bool mult_set = false;
    EH_DECLARE();

    float mult; // Scale as compared to previous stage
    float scale; // Scale as compared to first stage
};

#endif
