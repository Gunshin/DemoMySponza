#version 430

uniform sampler2DRect sampler_focus_depth;
uniform sampler2DRect sampler_depth;

uniform sampler2D sampler_texture;

uniform vec2 near_far;

out vec4 out_colour;


float lensDiameter = 20;



void main(void)
{

	vec2 pixelCoord = vec2(gl_FragCoord.xy/textureSize(sampler_texture, 0));
	ivec2 depthCoord = ivec2(gl_FragCoord.xy);

	float p33 = (near_far.y + near_far.x) / (near_far.x - near_far.y);
	float p34 = ((-2 * near_far.y) * near_far.x) / (near_far.y - near_far.x);



	float focusDepth = (-p34 / (p33 + texelFetch(sampler_focus_depth, ivec2(0, 0)))); // only one pixel in this sampler

	float pixelDepth = (-p34 / (p33 + texelFetch(sampler_depth, depthCoord)));


	float CoC = abs(

		);


	float a = smoothstep(0.0f, 50.0f, 50.0f - abs(pixelDepth - focusDepth));
	out_colour = texture(sampler_texture, pixelCoord) * a;
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
