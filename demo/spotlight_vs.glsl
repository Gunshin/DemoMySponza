#version 430

struct Light
{
    vec3 position;
    float range;
    vec3 direction;
    float half_cone_angle_degrees;
    vec3 intensity;
};

layout(std140, binding = 0) buffer BufferRender
{
    mat4 projectionViewMat;
    vec3 camPosition;
};

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

uniform vec3 lightPosition;
uniform float coneAngleDegrees;
uniform vec3 direction;
uniform float range;
uniform vec3 intensity;

uniform mat4 shadowBiasProjectionViewMat;

out Light vs_light;
out vec4 vs_shadow_coord;

void main(void)
{

    Light light;
    light.position = lightPosition;
    light.range = range;
    light.direction = direction;
    light.half_cone_angle_degrees = coneAngleDegrees / 2;
    light.intensity = intensity;
    vs_light = light;

    vec4 modelSpace = vec4((vertexPosition * range) + lightPosition, 1.0);

    gl_Position = projectionViewMat * modelSpace;

    vs_shadow_coord = shadowBiasProjectionViewMat * modelSpace;
}
