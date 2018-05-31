#version 440

layout(rgba8, binding = 0) uniform readonly image2D layer0;
layout(rgba8, binding = 1) uniform readonly image2D layer1;
layout(rgba8, binding = 2) uniform readonly image2D layer2;
layout(rgba8, binding = 3) uniform readonly image2D layer3;
layout(rgba8, binding = 4) uniform readonly image2D layer4;
layout(rgba8, binding = 5) uniform readonly image2D layer5;
layout(rgba8, binding = 6) uniform readonly image2D layer6;
layout(rgba8, binding = 7) uniform writeonly image2D dst;
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

uniform int nLayers;
uniform bool[5] active;
uniform ivec2[5] dimensions;
uniform int[5] blendingModes;

void main(){
	const ivec2 xy = ivec2(gl_GlobalInvocationID.xy);
	vec3 colors[7];
	colors[0] = imageLoad(layer0, xy).rgb;
	colors[1] = imageLoad(layer1, xy).rgb;
	colors[2] = imageLoad(layer2, xy).rgb;
	colors[3] = imageLoad(layer3, xy).rgb;
	colors[4] = imageLoad(layer4, xy).rgb;
	colors[5] = imageLoad(layer5, xy).rgb;
	colors[6] = imageLoad(layer6, xy).rgb;
	vec3 blended = vec3(0.0);
	for (int i = 0; i < nLayers; i++) {
		if (active[i]) {
			switch(blendingModes[i]) {
				case 1: // Multiply
					blended = blended * colors[i];
					break;
				case 2: // Screen
					blended = 1.0 - (1.0 - blended) * (1.0 - colors[i]);
					break;
				case 3: // Darken
					blended = min(blended, colors[i]);
					break;
				case 4: // Lighten
					blended = max(blended, colors[i]);
					break;
				case 5: // LinearDodge
					blended = blended + colors[i];
					break;
				case 6: // Difference
					blended = abs(blended - colors[i]);
					break;
				case 7: // Exclusion
					blended = blended + colors[i] + 2.0 * blended * colors[i];
					break;
				default:
					blended = colors[i];
					break;
			}
			blended = clamp(vec3(0.0), vec3(1.0), blended);
		}
	}
	imageStore(dst, xy, vec4(blended, 1.0));
}
