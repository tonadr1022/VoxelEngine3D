#version 400 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in float occlusionLevel;
layout (location = 3) in float texIndex;

out vec3 v_FragPos;
out vec2 v_TexCoord;
out float v_LightLevel;
flat out uint v_TexIndex;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform bool u_UseAmbientOcclusion;

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
    int x = int(texCoord.x);
    int y = int(texCoord.y);

    int textureYIndex = int(texIndex) % 16;
    int textureXIndex = int(texIndex) / 16;

    float u = (textureXIndex + x) * textureWidth;
    float v = (textureYIndex + y) * textureWidth;

    v_FragPos = vec3(u_Model * vec4(vertexPos, 1.0));
    gl_Position = u_Projection * u_View * u_Model * vec4(vertexPos, 1.0);
    v_TexCoord = vec2(x, y);
    v_TexIndex = int(texIndex);

    float occlusionFactor = mix(0.0, 1.0, float(u_UseAmbientOcclusion));
    float baseLightLevel = mix(1.0, 0.4, float(u_UseAmbientOcclusion));

    float occlusion = 0.2 * occlusionLevel * occlusionFactor;
    v_LightLevel = baseLightLevel + occlusion;

    //    float occlusionFactor = 1.0 - step(1, float(u_UseAmbientOcclusion));
    //    float baseLightLevel = 1.0 - step(0.8, float(u_UseAmbientOcclusion));
    //    float occlusion = 0.2 * occlusionLevel * occlusionFactor;
    //    v_LightLevel = baseLightLevel + occlusion;
}