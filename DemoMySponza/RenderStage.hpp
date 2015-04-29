#pragma once

#include <memory>
#include <map>
#include <tgl/tgl.h>


class RenderStage
{
public:

    RenderStage();
    virtual ~RenderStage();

    virtual void Execute() = 0;

    virtual void WindowViewReset(const int width_, const int height_) = 0;

    void SetHandlesMap(std::shared_ptr<std::map<std::string, GLuint>> handles_);

protected:

    std::shared_ptr<std::map<std::string, GLuint>> handles;

private:

};