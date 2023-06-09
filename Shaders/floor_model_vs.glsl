#version 310 es
precision mediump float;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 aTexCoords;
//out vec2 TexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float height;
out vec3 position;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    //TexCoords = aTexCoords;  
    height = aPos.y;  
    position = aPos;

    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}