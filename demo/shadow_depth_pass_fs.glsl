#version 430

layout(location = 0) out float fragmentdepth;

void main(void)
{
	fragmentdepth = gl_FragCoord.z / gl_FragCoord.w;
}
