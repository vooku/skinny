#version 440

layout(binding = 0) uniform sampler2DRect previousPass;

in vec2 texCoordVarying;

uniform float subsamplingFactor;

out vec4 outputColor;

//--------------------------------------------------------------
void main()
{
    outputColor = vec4(texture(previousPass, texCoordVarying * subsamplingFactor).rgb, 1.0f);
}
