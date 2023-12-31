#version 400 core

layout(location = 0) in uint vertexData; // assuming layout(location = 0) corresponds to your vertex data

out vec3 v_FragPos;
out vec2 v_TexCoord;
out vec3 v_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform vec3 normals[6] = vec3[6](
vec3(1.0, 0.0, 0.0),  // Front
vec3(-1.0, 0.0, 0.0), // Back
vec3(0.0, -1.0, 0.0), // Left
vec3(0.0, 1.0, 0.0),  // Right
vec3(0.0, 0.0, 1.0),  // Top
vec3(0.0, 0.0, -1.0)  // Bottom
);

const int atlasWidth = 16;
const float textureWidth = 1.0 / float(atlasWidth);


void main() {
    int xPos = int(bitfieldExtract(vertexData, 27, 5));
    int yPos = int(bitfieldExtract(vertexData, 22, 5));
    int zPos = int(bitfieldExtract(vertexData, 13, 9));
    int faceNum = int(bitfieldExtract(vertexData, 10, 3));
    int x = int(bitfieldExtract(vertexData, 9, 1));
    int y = int(bitfieldExtract(vertexData, 8, 1));

    int texIndex = int(bitfieldExtract(vertexData, 0, 8));

    int textureYIndex = int(texIndex) % 16;
    int textureXIndex = int(texIndex) / 16;

    float u = (textureXIndex + x) * textureWidth;
    float v = (textureYIndex + y) * textureWidth;

    vec3 vertexPos = vec3(xPos, yPos, zPos);
    v_FragPos = vec3(u_Model * vec4(vertexPos, 1.0));
    gl_Position = u_Projection * u_View * u_Model * vec4(vertexPos, 1.0);
    v_TexCoord = vec2(u, v);
    v_Normal = normals[faceNum];
}