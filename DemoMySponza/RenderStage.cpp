

#include "RenderStage.hpp"

RenderStage::RenderStage()
{

}

RenderStage::~RenderStage()
{

}

void RenderStage::SetHandlesMap(std::shared_ptr<std::map<std::string, GLuint>> handles_)
{
    handles = handles_;
}