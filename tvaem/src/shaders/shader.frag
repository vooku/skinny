#version 440
layout(binding = 1) uniform sampler2DRect layer0;
//layout(rgba8, binding = 1) uniform readonly image2D layer1;
//layout(rgba8, binding = 2) uniform readonly image2D layer2;
//layout(rgba8, binding = 3) uniform readonly image2D layer3;
//layout(rgba8, binding = 4) uniform readonly image2D layer4;
//layout(rgba8, binding = 5) uniform readonly image2D layer5;
//layout(rgba8, binding = 6) uniform readonly image2D layer6;

in vec2 uv;

uniform int nLayers;
uniform bool[7] playing;
uniform ivec2[7] dimensions;
uniform int[7] blendingModes;
uniform float[7] alphas;
uniform float masterAlpha;
uniform bool invert;
uniform bool reducePalette;
uniform bool colorShift;
uniform bool colorShift2;
out vec4 outputColor;

void main()
{
    vec3 colors[7];
    colors[0] = texture(layer0, uv).rgb;
    outputColor = vec4(1 - colors[0], 1.0);
}