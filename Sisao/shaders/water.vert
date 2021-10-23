#version 330

uniform mat4 modelviewprojection;
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

in vec2 position;

void main()
{
	gl_Position = modelviewprojection * vec4(position, 0.0, 1.0);;
}

