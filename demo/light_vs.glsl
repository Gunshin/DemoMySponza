#version 430

struct Light
{
    vec3 position;
    float range;
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
layout (location = 3) in float lightRange;
layout (location = 4) in vec3 lightIntensity;

out Light vs_light;

void main(void)
{
    Light light;
    light.position = lightPosition;
    light.range = lightRange;
    light.intensity = lightIntensity;
	vs_light = light;

	gl_Position = projectionViewMat * vec4((vertexPosition * lightRange) + lightPosition, 1.0);
}
