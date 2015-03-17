#version 430

struct DirectionalLight
{
    vec3 direction;
    vec3 intensity;
};

layout(std140, binding = 0) buffer BufferLights
{
    DirectionalLight lights[];
};


uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;
uniform sampler2DRect sampler_world_mat;

uniform vec3 directional_light;
uniform vec3 light_intensity;

out vec3 reflected_light;

vec3 AddDirectionalLight(vec3 direction_, vec3 intensity_, vec3 normal_);

void main(void)
{
    ivec2 pixelCoord = ivec2(gl_FragCoord.xy);
    vec3 normal = texelFetch(sampler_world_normal, pixelCoord).xyz;
    vec3 mat = texelFetch(sampler_world_mat, pixelCoord).xyz;

    for(int i = 0; i < lights.length; ++i)
    {
        vec3 directionalLightColour = AddDirectionalLight(-directional_light, light_intensity, normal);

        reflected_light += directionalLightColour * mat;
    }


    
}

vec3 AddDirectionalLight(vec3 direction_, vec3 intensity_, vec3 normal_)
{
    vec3 L = normalize(direction_);

    return vec3(1) * max(dot(L, normal_), 0) * intensity_;
}