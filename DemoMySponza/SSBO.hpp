#pragma once

#include <functional>

#include <tgl/tgl.h>

class ShaderProgram;

class SSBO
{

public:

    SSBO();
    SSBO(std::function<bool(GLuint)> onFillData_);

    bool AttachToProgram(const unsigned int channelID_, const ShaderProgram &program_, const std::string &bufferName_);

    bool FillData();

    ~SSBO();

private:

    GLuint bufferID;

    std::function<bool(GLuint)> onFillData;

};