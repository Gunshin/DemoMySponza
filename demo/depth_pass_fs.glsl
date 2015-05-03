#version 430

uniform sampler2DRect sampler_focus_depth;
uniform sampler2DRect sampler_depth;

uniform sampler2D sampler_texture;

uniform vec2 near_far; // x: near, y: far
uniform vec2 p; //x: p33, y: p34

uniform int horizontal;

out vec4 out_colour;

// using 
float lensDiameter = 0.1f;
float focalDepth = 3.0f;
int numSamples = 1;

float LinearDepth(float d_)
{
	return (-p.y / (p.x * d_)) * 100;
}

float CoC(float focalDistance_, float samplePixelDistance_)
{
	return abs(
		lensDiameter * (focalDepth * ((focalDistance_ - samplePixelDistance_) / (samplePixelDistance_ * (focalDistance_ - focalDepth))))
		);

	// (samplePixelDistance_ * (focalDistance_ - focalDepth)) == -2 * (2 - 2.1) = -0.2
	// (focalDistance_ - samplePixelDistance_) == 2 - -2 = 4
	//((focalDistance_ - samplePixelDistance_) / (samplePixelDistance_ * (focalDistance_ - focalDepth))) == 4 / -1 = -4
	// 3 * 2 == 6
	// 50 * 6 == 300
}

float OverlapFunc(float pCoCRadius_, float pDistanceFromCenter_, float pp1DistanceFromCenter_)
{
	if (pCoCRadius_ <= pDistanceFromCenter_)
	{
		return 0;
	}
	else if (pDistanceFromCenter_ <= pCoCRadius_ && pCoCRadius_ < pp1DistanceFromCenter_)
	{
		return pCoCRadius_ - pDistanceFromCenter_;
	}

	return 1;
}

float IntensityFunc(float radius_)
{
	return 1 / ((radius_ * radius_) + 0.05f);
}

float IntensityLeakageControlFunc(float samplePixelDepth_, float focalDistance_, float centerPixelCoC_)
{
	// potential performance improvement with 'step'
	if (samplePixelDepth_ > focalDistance_)
	{
		centerPixelCoC_;
	}
	
	return 1;

}

void main(void)
{

	int vertical = horizontal == 0 ? 1 : 0;

	vec2 pixelCoord = vec2(gl_FragCoord.xy / textureSize(sampler_texture, 0));
	ivec2 depthCoord = ivec2(gl_FragCoord.xy);

	float focalDistance = -1 * LinearDepth(texelFetch(sampler_focus_depth, ivec2(0, 0))); // only one pixel in this sampler
	//float focalDistance = -100.0f;

	float currentPixelDistance = LinearDepth(texelFetch(sampler_depth, depthCoord)); // temp distance

	// focalDistance = 2
	//currentPixelDistance = -2

	float sampleCenterPixelCoC = CoC(focalDistance, currentPixelDistance);

	float totalWeight = IntensityFunc(sampleCenterPixelCoC);


	vec3 centerPixelColour = texture(sampler_texture, pixelCoord).xyz;


	for (int i = -numSamples; i < numSamples + 1; ++i)
	{
		if (i == 0) continue;

		ivec2 coord = depthCoord + ivec2(i * horizontal, i * vertical);
		vec2 samplerCoord = vec2((gl_FragCoord.xy + ivec2(i * horizontal, i * vertical)) / textureSize(sampler_texture, 0));

		float samplePixelDistance = LinearDepth(texelFetch(sampler_depth, coord));
		float samplePixelCoC = CoC(focalDistance, samplePixelDistance);

		

		vec3 samplePixelColour = texture(sampler_texture, samplerCoord).xyz;


		float samplePixelOverlap = OverlapFunc(samplePixelCoC, abs(i * 0.0000001f), abs((i + 1) * 0.0000001f));

		float samplePixelIntensity = IntensityFunc(samplePixelCoC);

		float samplePixelLeakage = IntensityLeakageControlFunc(samplePixelDistance, focalDistance, sampleCenterPixelCoC);



		float sampleWeight = samplePixelOverlap * samplePixelIntensity * samplePixelLeakage;
		//float sampleWeight = samplePixelOverlap;
		totalWeight += sampleWeight;

		centerPixelColour += samplePixelColour * sampleWeight;

	}

	centerPixelColour /= totalWeight;
	out_colour = vec4(focalDistance, currentPixelDistance, sampleCenterPixelCoC, 1);


	//out_colour = vec4(focalDistance, currentPixelDistance, sampleCenterPixelCoC, totalWeight);
	
	//out_colour = texture(sampler_texture, vec2((gl_FragCoord.xy + ivec2(1 * horizontal, 1 * vertical)) / textureSize(sampler_texture, 0)));

}
