#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;


void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(vertexPos, 1.0);
    v_TexCoord = vec2(texCoord.x, texCoord.y);
}