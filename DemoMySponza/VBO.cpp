#include "VBO.hpp"

VBO::VBO()
{

}

VBO::VBO(std::function<bool(GLuint)> onFillData_)
{
    onFillData = onFillData_;
}

VBO::~VBO()
{

}

bool VBO::GenerateBuffer()
{
    glGenBuffers(1, &bufferID);

    return FillData();
}

bool VBO::FillData()
{
    return onFillData(bufferID);
}

GLuint VBO::GetVBOID() const
{
    return bufferID;
}

unsigned int VBO::GetSize() const
{
    return size;
}