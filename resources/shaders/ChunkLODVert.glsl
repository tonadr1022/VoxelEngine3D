#version 400 core

layout (location = 0) in uint vertexData1;
layout (location = 1) in uint vertexData2;

out vec3 v_TexCoord;
uniform mat4 u_MVP;

void main() {
    uint posX = bitfieldExtract(vertexData1, 0, 6);
    uint posY = bitfieldExtract(vertexData1, 6, 6);
    uint posZ = bitfieldExtract(vertexData1, 12, 6);
    int u = int(bitfieldExtract(vertexData1, 18, 6));
    int v = int(bitfieldExtract(vertexData1, 24, 6));
    gl_Position = u_MVP * vec4(posX, posY, posZ, 1.0);
    v_TexCoord = vec3(u, v, vertexData2);
}