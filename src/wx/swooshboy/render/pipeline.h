#ifndef VBA_PIPELINE_H
#define VBA_PIPELINE_H
#include "swooshboy.h"
#include "vbaerr.h"
#include "shader/glsl.h"

class renderStage {
    friend class vbaGL;
    friend class renderPipeline;
public:
    renderStage(vbaGL *globj);

    // Needs to be protected
    vbaTex *texture; //Texutre we are rendering TO

protected:
    virtual bool setIndex(uint idx);
    virtual bool render(vbaTex *src);

    bool setMult(uint coeff);

    uint index;
    glslProg *shader;
    GLuint buffer;

    vbaGL *ctx;

    bool init_t = false;
    bool init_s = false;
    bool init_b = false;
private:
    bool init_m = false;
    EH_DECLARE();

    float mult; // Scale as compared to previous stage
    float scale; // Scale as compared to first stage
};

class renderPipeline {
    friend class vbaGL;
    friend class renderStage;
public:
    renderPipeline(vbaGL *globj);
    bool addStage(renderStage *stg);
    bool removeStage(uint idx);
    bool refreshStages();
    uint getSize();
    bool render(const void* data);
    bool draw();

    // Make this private
    std::deque<renderStage *> pipeline;

private:

    EH_DECLARE();

    vbaTex *base;

    glslProg *shd_draw; //Should be passthrough shader

    vbaGL *ctx;
};


#endif
