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
layout (location = 2) in vec3 lightPosition;
layout (location = 3) in float coneAngleDegrees;
layout (location = 4) in vec3 direction;
layout (location = 5) in float range;
layout (location = 6) in vec3 intensity;

out Light vs_light;

void main(void)
{

    Light light;
    light.position = lightPosition;
    light.range = range;
    light.direction = direction;
    light.half_cone_angle_degrees = coneAngleDegrees;
    light.intensity = intensity;
    vs_light = light;

	gl_Position = projectionViewMat * vec4((vertexPosition * range) + lightPosition, 1.0);
}
