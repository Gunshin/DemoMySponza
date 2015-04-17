#pragma once

#include <functional>

#include <tgl/tgl.h>

class VBO
{

public:

    VBO();
    VBO(std::function<bool(GLuint)> onFillData_);

    ~VBO();

    bool GenerateBuffer();

    bool FillData();

    GLuint GetVBOID() const;

	void SetSize(const unsigned int size_);
    unsigned int GetSize() const;

private:

    GLuint bufferID;
    unsigned int size;

    std::function<bool(GLuint)> onFillData;

};