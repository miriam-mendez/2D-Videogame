#version 330

uniform mat4 modelviewprojection;
in vec2 position;
out vec4 fragPosition;

void main()
{
	fragPosition = modelviewprojection * vec4(position, 0.0, 1.0);
	gl_Position = fragPosition;
}

