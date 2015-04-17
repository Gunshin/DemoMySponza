#include "SSBO.hpp"

#include "ShaderProgram.hpp"

SSBO::SSBO()
{

}

SSBO::SSBO(std::function<bool(GLuint)> onFillData_)
{
    onFillData = onFillData_;

    glGenBuffers(1, &bufferID);

    FillData();
}

bool SSBO::AttachToProgram(const unsigned int channelID_, const ShaderProgram &program_, const std::string &bufferName_)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, channelID_, bufferID);
    glShaderStorageBlockBinding(
        program_.getProgramID(),
        glGetUniformBlockIndex(program_.getProgramID(), bufferName_.c_str()),
        channelID_);

    return true;
}

bool SSBO::FillData()
{
    return onFillData(bufferID);
}

SSBO::~SSBO()
{

}