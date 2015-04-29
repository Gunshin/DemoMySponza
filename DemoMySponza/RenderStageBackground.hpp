#pragma once


#include "RenderStage.hpp"
#include "ShaderProgram.hpp"

class RenderStageBackground : public RenderStage
{

public:

    RenderStageBackground();
    virtual ~RenderStageBackground();

    virtual void Execute();

    virtual void WindowViewReset(const int width_, const int height_);

private:

    ShaderProgram program;

};