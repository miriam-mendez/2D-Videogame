#version 330

uniform vec4 color;
uniform float time;

out vec4 outColor;

const float speed = 0.0005;
const float wave_size = 20;
const float wave_top = 224;
const float wave_bottom = 0;

void main()
{
	vec2 wpos = gl_FragCoord.xy;

	float wh = 4 *sin(time*speed + wpos.x*0.01) + 2*sin(time*speed*5 + wpos.x*0.01);
	float size = wave_size * (wh + 5) / 10.0; // map [-1, 1] to [0, wave_size]
	float wave_h = wave_top - size;

	float depth_visibility = 3 * wpos.y/560;
	float a = max(color.a*1.5, 0);
	vec4 xx = mix(vec4(0.11,0.11,0.13,a), color, depth_visibility* depth_visibility);
	
	if (wpos.y < wave_h) {
		outColor = xx;
	} else {
		discard;
	}
	
}

