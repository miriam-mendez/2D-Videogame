#version 330

uniform vec4 color;
uniform float time;

out vec4 outColor;
in vec4 t;

void main()
{
	float x = t.x + 0.001 * 0.05;
	float w = t.y + sin(x*0.01)*10;
	/*if (w < 0) {
		outColor = color;
	}
	else {
		discard;
	}*/
	if (t.y > 200) {
		outColor = color;
	}
	else {
		discard;
	}
}

