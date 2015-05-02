#version 430

uniform sampler2DRect sampler_focus_depth;
uniform sampler2DRect sampler_depth;

uniform sampler2D sampler_texture;

out vec4 out_colour;

void main(void)
{

	ivec2 pixelCoord = ivec2(gl_FragCoord.xy);

	float focusDepth = texelFetch(sampler_focus_depth, ivec2(0, 0)); // only one pixel in this sampler

	

	float pixelDepth = texelFetch(sampler_depth, pixelCoord);

	//out_colour = vec4(pixelDepth - focusDepth, 0, 0, 1);

	float a = 1 - smoothstep(0.0f, 0.05f, abs(pixelDepth - focusDepth));
	out_colour = a * texture(sampler_texture, pixelCoord);
	/*if (abs(pixelDepth - focusDepth) < 0.05)
	{
		

		
	}
	else
	{
		out_colour = vec4(0, 0, 0, 1);
	}*/

	/*for (int i = -1; i < 2; ++i)
	{

		for (int j = -1; j < 2; ++j)
		{

			depthTotal += texelFetch(sampler_depth, ivec2(i, j) + centre).r;

		}

	}*/


}
