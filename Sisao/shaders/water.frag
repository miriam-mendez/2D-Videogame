#version 330

uniform vec4 color;
uniform float time;

out vec4 outColor;

const float speed = 0.0005;
const float ycenter = 448.0 / 2.0;
const float wave_size = 7;

void main()
{
	vec2 fpos = gl_FragCoord.xy;
	float w = 4*sin(time*speed + fpos.x*0.01) + 2*sin(time*speed*5 + fpos.x*0.01);
	w = wave_size * w / 6; // 4+2

	float wave_height = ycenter + w;
	
	if (fpos.y < wave_height) {
		float depth_visibility = 4 * fpos.y/560;
		float visibility = clamp(depth_visibility * depth_visibility, 0, 1);
		vec4 c = mix(vec4(0.09,0.09,0.14,0.75), color, visibility);
		outColor = c;
	}
	else {
		discard;
	}	
}

