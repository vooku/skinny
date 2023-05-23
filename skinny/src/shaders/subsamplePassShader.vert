#version 440

in vec4 position;
uniform mat4 modelViewProjectionMatrix;
uniform float subsamplingRateX;
uniform float subsamplingRateY;
out vec2 texCoordVarying;

//--------------------------------------------------------------
void main(){
    texCoordVarying = position.xy * vec2(subsamplingRateX, subsamplingRateY); // y no in texcoord??
    gl_Position = modelViewProjectionMatrix * position;
}
