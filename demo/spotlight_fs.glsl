#version 430 core

struct Light
{
    vec3 position;
    float range;
    vec3 direction;
    float half_cone_angle_degrees;
    vec3 intensity;
};

// updated every frame (or render call)
layout(std140, binding = 0) buffer BufferRender
{
    mat4 projectionViewMat;
    vec3 camPosition;
};

vec3 calculateColour(Light light_, vec3 materialColour_, float shininess_, vec3 V_, vec3 pos_, vec3 normal_);

uniform sampler2DRect sampler_world_position;
uniform sampler2DRect sampler_world_normal;
uniform sampler2DRect sampler_world_mat;

uniform sampler2DRect shadow_depths;

uniform mat4 shadowProjectionViewMat;

uniform int shadows;

in Light vs_light;

out vec3 reflected_light;

void main(void)
{

    ivec2 pixelCoord = ivec2(gl_FragCoord.xy);
    vec3 position = texelFetch(sampler_world_position, pixelCoord).xyz;
    vec3 normal = texelFetch(sampler_world_normal, pixelCoord).xyz;
	vec4 matColour = texelFetch(sampler_world_mat, pixelCoord).rgba; // the alpha value is the shininess of the material

	vec3 V = normalize(camPosition - position);

	float visibility = 1.0f;

	if (shadows == 1)
	{
		vec4 lightCoord = shadowProjectionViewMat * vec4(position, 1.0f);

		lightCoord.xyz = ((lightCoord.xyz / lightCoord.w) + 1.0f) * 0.5f;

		ivec2 lastLightCoord = ivec2(lightCoord.xy * textureSize(shadow_depths, 0));

		float distanceToLight = texelFetch(shadow_depths, lastLightCoord).r;


		if (distanceToLight - 0.0001f < lightCoord.z)
		{
			visibility = 0.0f;
		}

	}

	vec3 col = calculateColour(vs_light, matColour.rgb, matColour.a, V, position, normal);

    reflected_light = visibility * col * vs_light.intensity;

}

vec3 calculateColour(Light light_, vec3 materialColour_, float shininess_, vec3 V_, vec3 pos_, vec3 normal_)
{
	
	vec3 L = normalize(light_.position - pos_);

	float LDdotML = dot(normalize(light_.direction), normalize(-L));

	if(LDdotML > cos(light_.half_cone_angle_degrees))
	{
		vec3 R = normalize(reflect(-L, normal_));

		float distance = distance(pos_, light_.position);

		vec3 attenuatedDistance = vec3(1.0, 1.0, 1.0) * smoothstep(0, light_.range, light_.range - distance);
        vec3 attenuatedCone = vec3(1.0, 1.0, 1.0) * smoothstep(cos(light_.half_cone_angle_degrees), 1, LDdotML);

        vec3 attenuatedLight = attenuatedDistance * attenuatedCone;

		vec3 Id = materialColour_ * max(dot(L, normal_), 0) * attenuatedLight;

		vec3 Is = vec3(0, 0, 0);
		if(dot(L, normal_) > 0 && shininess_ > 0)
		{
			Is = vec3(1, 1, 1) * pow(max(0, dot(R, V_)), shininess_) * attenuatedLight;
		}

		return Id + Is;
	}

	return vec3(0,0,0);
}