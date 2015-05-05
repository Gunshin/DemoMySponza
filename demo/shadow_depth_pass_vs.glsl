#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in mat4x3 instanceMat;
layout(location = 6) in int matIndex;

uniform mat4 projectionViewMat;

void main(void)
{
    vec4 pos = vec4(instanceMat * vec4(position, 1), 1);

    gl_Position = projectionViewMat * pos;
}
