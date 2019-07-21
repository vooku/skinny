#version 440

uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec2 texcoord;

out vec2 uv;

void main(){
    uv = position.xy * vec2(640.0 / 1920.0, 360.0 / 1080);
    gl_Position = modelViewProjectionMatrix * position;
}
