#version 310 es
precision mediump float;
layout (location = 0) in vec3 aPos;

//uniform mat4 model;        // transition from local space to world space
uniform mat4 view;         // transition from world space to view space
uniform mat4 projection;   // transition from view space to clip space

void main()
{  
    gl_Position = projection * view * vec4(aPos, 1.0);
}
