#version 430

uniform sampler2DRect sampler_depth;

uniform vec2 dimensions;

out vec4 out_colour;

void main(void)
{

	ivec2 centre = ivec2(dimensions / 2);

	float depthTotal = 0;

	for (int i = -1; i < 2; ++i)
	{

		for (int j = -1; j < 2; ++j)
		{

			depthTotal += texelFetch(sampler_depth, ivec2(i, j) + centre).r;

		}

	}

	out_colour = vec4(depthTotal / 9, 0, 0, 0);

}
