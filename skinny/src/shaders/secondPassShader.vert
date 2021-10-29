#version 440

in vec4 position;
uniform mat4 modelViewProjectionMatrix;

out vec2 texCoordVarying;

//--------------------------------------------------------------
void main(){
    texCoordVarying = position.xy; // y no in texcoord??
    gl_Position = modelViewProjectionMatrix * position;
}
