#version 440

layout(rgba8, binding = 0) uniform readonly image2D layer0;
layout(rgba8, binding = 1) uniform readonly image2D layer1;
layout(rgba8, binding = 2) uniform writeonly image2D dst;
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

uniform float[2] alphas;

void main(){
	const ivec2 xy = ivec2(gl_GlobalInvocationID.xy);
	const vec4 c0 = imageLoad(layer0, xy);
	const vec4 c1 = imageLoad(layer1, xy);
	imageStore(dst, xy, clamp(vec4(0.0), vec4(1.0), c0 * alphas[0] + c1 * alphas[1]));
}
