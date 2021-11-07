#version 330

uniform vec4 color;
uniform float time;

in vec4 fragPosition; // (ClipSpace: -1 to 1)
out vec4 outColor;

uniform float amp = 0.005;
uniform float freq = 0.00025; // Hz
uniform float freq2 = 0.0008; // Hz
const float pi = 3.141592;

void main()
{
	vec2 pos = fragPosition.xy;
	float y = amp*sin(2*pi*freq*time+10*pos.x) + amp/2*sin(2*pi*freq2*time+10*pos.x);

	if (pos.y  + y < 0) {
		float depth_visibility = pos.y;
		float visibility = clamp(depth_visibility * depth_visibility, 0, 1);
		vec4 c = mix(vec4(0.09,0.09,0.14,0.75), color, 1-visibility);
		c.rgb = mix(vec3(0.5), c.rgb, 1.4); // increments the contrast
		outColor = c;
	}
	else {
		discard;
	}	
}
