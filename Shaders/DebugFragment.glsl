#version 310 es
precision mediump float;
out vec4 FragColor;

uniform vec4 color; // we set this variable in the OpenGL code.

void main()
{    
    FragColor = color;
}