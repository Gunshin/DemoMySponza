#version 330

in vec3 vertex_position;
in vec3 vertex_normal;

void main(void)
{
    gl_Position = vec4(vertex_position, 1.0);
}
