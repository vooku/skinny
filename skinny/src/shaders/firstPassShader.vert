#version 440

const int n = 8;

in vec4 position;

uniform vec2[n] dimensions;
uniform ivec2 screenSize;
uniform mat4 modelViewProjectionMatrix;

out vec2[n] uvs;

//--------------------------------------------------------------
void main(){
    for (int i = 0; i < n; i++)
    {
        uvs[i] = position.xy * dimensions[i] / screenSize;
    }

    gl_Position = modelViewProjectionMatrix * position;
}
