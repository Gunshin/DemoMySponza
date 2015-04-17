#include "MeshBuffer.hpp"

MeshBuffer::MeshBuffer()
{
}

bool MeshBuffer::GenerateBuffers()
{
    glGenBuffers(1, &vertexVBO);
    glGenBuffers(1, &elementVBO);

    return true;
}

const MeshBuffer::Mesh MeshBuffer::AddMesh(tsl::IndexedMesh mesh_)
{
    Mesh newMesh;

    newMesh.startVerticeIndex = vertices.size();
    newMesh.startElementIndex = elements.size();

    for (unsigned int j = 0; j < mesh_.vertex_array.size(); ++j)
    {
        vertices.push_back(Vertex(
            ConvVec3(mesh_.vertex_array[j]),
            ConvVec3(mesh_.normal_array[j])
            ));
    }

    // why is the element array a vector<InstanceId>?
    std::vector<int> elementArray = mesh_.index_array;
    for (unsigned int j = 0; j < elementArray.size(); ++j)
    {
        elements.push_back(elementArray[j]);
    }

    newMesh.endVerticeIndex = vertices.size() - 1;
    newMesh.endElementIndex = elements.size() - 1;
    newMesh.verticeCount = newMesh.endVerticeIndex - newMesh.startVerticeIndex;
    newMesh.element_count = newMesh.endElementIndex - newMesh.startElementIndex + 1;

    meshes.push_back(newMesh);

    return newMesh;
}

const MeshBuffer::Mesh MeshBuffer::AddMesh(SceneModel::Mesh mesh_)
{
    Mesh mesh;
    mesh.startVerticeIndex = vertices.size();
    mesh.startElementIndex = elements.size();

    // i store these temporarily since getPositionArray() will likely end up copying the whole array rather than passing the original
    std::vector<glm::vec3> positionArray = mesh_.getPositionArray();
    std::vector<glm::vec3> normalArray = mesh_.getNormalArray();

    for (unsigned int j = 0; j < positionArray.size(); ++j)
    {
        vertices.push_back(Vertex(positionArray[j],
            normalArray[j]));
    }

    // why is the element array a vector<InstanceId>?
    std::vector<unsigned int> elementArray = mesh_.getElementArray();
    for (unsigned int j = 0; j < elementArray.size(); ++j)
    {
        elements.push_back(elementArray[j]);
    }

    mesh.endVerticeIndex = vertices.size() - 1;
    mesh.endElementIndex = elements.size() - 1;
    mesh.verticeCount = mesh.endVerticeIndex - mesh.startVerticeIndex;
    mesh.element_count = mesh.endElementIndex - mesh.startElementIndex + 1;

    meshes.push_back(mesh);

    return mesh;
}

bool MeshBuffer::Flush()
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        elements.size() * sizeof(unsigned int),
        elements.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

GLuint MeshBuffer::GetVertexVBOID() const
{
    return vertexVBO;
}

GLuint MeshBuffer::GetElementVBOID() const
{
    return elementVBO;
}