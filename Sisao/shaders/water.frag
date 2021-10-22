#version 330

uniform vec4 color;
uniform float time;

out vec4 outColor;

void main()
{
	float x = gl_FragCoord.x + time * 0.05;
	float w = gl_FragCoord.y - sin(x*0.01)*10;
	if (w < 100) {
		outColor = color;
	}
	else {
		discard;
	}
}

