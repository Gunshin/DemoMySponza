#version 430

uniform sampler2DRect sampler_depth;

uniform vec2 dimensions;

out vec4 out_colour;

void main(void)
{

	ivec2 centre = ivec2(dimensions / 2);

	float depthTotal = 0;

	for (int i = -32; i < 32; ++i)
	{

		for (int j = -32; j < 32; ++j)
		{

			depthTotal += texelFetch(sampler_depth, ivec2(i, j) + centre).r;

		}

	}

	out_colour = vec4(depthTotal / (64 * 64), 0, 0, 0);

}
