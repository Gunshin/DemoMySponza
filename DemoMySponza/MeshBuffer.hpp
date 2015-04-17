#pragma once

#include <vector>

#include <tgl/tgl.h>
#include <SceneModel/SceneModel.hpp>
#include <tsl/primitives.hpp>

class MeshBuffer
{

public:

    struct Mesh
    {
        GLuint vao;// VertexArrayObject for the shape's vertex array settings
        GLuint instanceVBO;
        int startVerticeIndex, endVerticeIndex, verticeCount;
        int startElementIndex, endElementIndex, element_count; // Needed for when we draw using the vertex arrays

        Mesh() : startVerticeIndex(0),
            endVerticeIndex(0),
            verticeCount(0),
            startElementIndex(0),
            endElementIndex(0),
            element_count(0)
        {
        }
    };

    struct Vertex
    {
        Vertex() {};
        Vertex(glm::vec3 pos_, glm::vec3 norm_) : position(pos_), normal(norm_) {}
        glm::vec3 position, normal;
    };

    MeshBuffer();

    bool GenerateBuffers();

    const Mesh AddMesh(tsl::IndexedMesh mesh_);
    const Mesh AddMesh(SceneModel::Mesh mesh_);

    bool Flush();

    GLuint GetVertexVBOID() const;
    GLuint GetElementVBOID() const;

private:

    GLuint vertexVBO; // VertexBufferObject for the vertex positions
    GLuint elementVBO; // VertexBufferObject for the elements (indices)

    std::vector<Mesh> meshes;

    std::vector<Vertex> vertices;
    std::vector< unsigned int > elements;



    // method fixes damn inconsistencies of this so called 'legacy code'
    inline glm::vec3 ConvVec3(tsl::Vector3 &vec_)
    {
        return glm::vec3(vec_.x, vec_.y, vec_.z);
    }

};