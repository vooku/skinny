#version 440

in vec2 texCoordVarying;

layout(binding = 0) uniform sampler2DRect previousPass;
layout(binding = 1) uniform sampler2DRect loadingScreen;

uniform bool playing;

out vec4 outputColor;

//--------------------------------------------------------------
void main()
{
    vec3 blended = texture(previousPass, texCoordVarying).rgb;
    
    if (playing)
    {
        vec3 screen =  texture(loadingScreen, texCoordVarying).rgb;
        blended += screen;
        blended = clamp(blended, vec3(0.0), vec3(1.0));
    }

    outputColor = vec4(blended, 1.0);
}
