#version 440

layout(rgba8, binding=0) uniform readonly image2D layer0;
layout(rgba8, binding=1) uniform readonly image2D layer1;
layout(rgba8, binding=2) uniform writeonly image2D dst;
layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

void main(){
	ivec2 xy = ivec2(gl_GlobalInvocationID.xy);
	vec4 c0 = imageLoad(layer0, xy);
	vec4 c1 = imageLoad(layer1, xy);
	imageStore(dst, xy, clamp(vec4(0.0), vec4(1.0), c0 + c1));
	//imageStore(dst, xy, vec4(gl_LocalInvocationID / vec3(gl_WorkGroupSize), 1.0));
	//imageStore(dst, xy, vec4(gl_GlobalInvocationID / vec3(1920.0, 1080.0, 1.0), 1.0));
}
