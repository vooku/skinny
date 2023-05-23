#version 440

layout(binding = 0) uniform sampler2DRect previousPass;

in vec2 texCoordVarying;

uniform float subsamplingRateX;
uniform float subsamplingRateY;

out vec4 outputColor;

//--------------------------------------------------------------
void main()
{
    vec2 samplingRate = vec2(subsamplingRateX, subsamplingRateY);
    outputColor = vec4(texture(previousPass, texCoordVarying * samplingRate).rgb, 1.0f);
}
