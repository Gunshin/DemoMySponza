#pragma once

#include <SceneModel/SceneModel_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "ShaderProgram.hpp"
#include "QueryTimer.hpp"
#include "SSBO.hpp"
#include "MeshBuffer.hpp"
#include "VBO.hpp"

class MyView : public tygra::WindowViewDelegate
{
public:

    MyView();

    ~MyView();

    void
    setScene(std::shared_ptr<const SceneModel::Context> scene);

private:

    void
    windowViewWillStart(std::shared_ptr<tygra::Window> window) override;

    void
    windowViewDidReset(std::shared_ptr<tygra::Window> window,
                       int width,
                       int height) override;

    void
    windowViewDidStop(std::shared_ptr<tygra::Window> window) override;

    void
    windowViewRender(std::shared_ptr<tygra::Window> window) override;

    std::shared_ptr<const SceneModel::Context> scene_;

    float aspectRatio;

    MeshBuffer meshBuffer;


    std::vector< MeshBuffer::Mesh > loadedMeshes;

    SSBO materialSSBO, renderSSBO, directionalLightsSSBO;

    struct MaterialData
    {
        glm::vec3 colour;
        float shininess;
    };
    /*
    generate a map which contains the MaterialID as the key, which leads to the index inside of my vector that the material is contained
    */
    std::map<SceneModel::MaterialId, unsigned int> mapMaterialIndex;

    struct InstanceData
    {
        glm::mat4x3 positionData;
        GLint materialDataIndex;
    };
    //std::vector< std::vector< InstanceData > > instanceData;
    std::vector< VBO> vboInstances;

    // cant get access to the MyScene::Light since we are only declaring MyScene as a class (no direct reference)
    struct PointLightData
    {
        glm::vec3 position;
        float range;
        glm::vec3 intensity;
    };
    std::vector<PointLightData> pointLights;
    GLuint bufferRender;
    MeshBuffer::Mesh pointLightMesh, globalLightMesh;

    struct SpotLightData
    {
        glm::vec3 position;
        float coneAngleDegrees;
        glm::vec3 direction;
        float range;
        glm::vec3 intensity;
    };
    std::vector<SpotLightData> spotLights;
    MeshBuffer::Mesh spotLightMesh;

    struct DirectionalLightData
    {
        glm::vec3 direction;
        float pack1;
        glm::vec3 intensity;
        float pack2;
    };
    GLuint bufferDirectionalLights;

    ShaderProgram lightProgram, firstPassProgram, globalLightProgram, backgroundProgram, postProcessProgram;

    GLuint gbufferFBO;
    GLuint gbufferTO[3];
    GLuint depthStencilRBO;

    GLuint lbufferFBO;
    GLuint lbufferTO;
    GLuint lbufferColourRBO;

    GLuint postProcessFBO;
    GLuint postProcessColourRBO;

    GLuint queryID;

    void SetBuffer(glm::mat4 projectMat_, glm::vec3 camPos_);
    void UpdatePointLights();
    void UpdateSpotLights();

    void GenerateShaderPrograms();
    void SetupSSBOS();
    void GenerateMeshes(const std::vector<SceneModel::Mesh> &meshes_);

    std::shared_ptr<QueryTimer> gbufferTimer, backgroundTimer, globalLightsTimer, lbufferTimer, postTimer;
    std::vector<GLuint64> gbufferTimes, backgroundTimes, globalLightsTimes, lbufferTimes, postTimes;

};
