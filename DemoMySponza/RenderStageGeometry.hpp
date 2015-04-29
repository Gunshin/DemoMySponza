#pragma once


#include "RenderStage.hpp"
#include "ShaderProgram.hpp"

class RenderStageGeometry : public RenderStage
{

public:

    RenderStageGeometry();
    virtual ~RenderStageGeometry();

    virtual void Execute();

    virtual void WindowViewReset(const int width_, const int height_);

private:

    ShaderProgram program;

};