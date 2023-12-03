#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform int u_TexIndex;



const int atlasWidth = 16;
const float textureWidth = 1.0 / float(atlasWidth);

void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(vertexPos, 1.0);

    int textureYIndex = int(u_TexIndex) % 16;
    int textureXIndex = int(u_TexIndex) / 16;

    float u = (textureXIndex + texCoord.x) * textureWidth;
    float v = (textureYIndex + texCoord.y) * textureWidth;

    v_TexCoord = vec2(u, v);

}