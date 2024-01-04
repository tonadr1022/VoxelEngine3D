#version 400 core

layout (location = 0) in uint vertexData;

out vec3 v_FragPos;
out vec3 v_TexCoord;
out float v_LightLevel;
flat out uint v_TexIndex;

uniform ivec2 u_ChunkWorldPos;
uniform float u_Time;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform bool u_UseAmbientOcclusion;

uniform vec3 normals[6] = vec3[6](
vec3(1.0, 0.0, 0.0), // Front
vec3(-1.0, 0.0, 0.0), // Back
vec3(0.0, -1.0, 0.0), // Left
vec3(0.0, 1.0, 0.0), // Right
vec3(0.0, 0.0, 1.0), // Top
vec3(0.0, 0.0, -1.0)// Bottom
);

const int atlasWidth = 16;
const float textureWidth = 1.0 / float(atlasWidth);


const float waveAmplitude = 0.1;
const float waveFrequency = 1.0;
const float dist = 1.0;
const float PI = 3.14285714286;

// only applies for texIndex 127 (water)
vec3 applyWave(vec3 vertexPos, uint texIndex) {
    if (texIndex != 127) {
        return vertexPos;
    }
    vec2 vertexWorldPos = vertexPos.xy + vec2(u_ChunkWorldPos.x, u_ChunkWorldPos.y);

    vec2 origin1 = vertexWorldPos + vec2(dist, dist);
    vec2 origin2 = vertexWorldPos + vec2(dist, -dist);
    vec2 origin3 = vertexWorldPos + vec2(-dist, dist);
    vec2 origin4 = vertexWorldPos + vec2(-dist, -dist);

    float distance1 = length(origin1);
    float distance2 = length(origin2);
    float distance3 = length(origin3);
    float distance4 = length(origin4);


    float wave = sin(3.3 * PI * distance1 * 0.13 + u_Time) * 0.1 +
    sin(3.2 * PI * distance2 * 0.12 + u_Time) * 0.1 +
    sin(3.1 * PI * distance3 * 0.24 + u_Time) * 0.1 +
    sin(3.5 * PI * distance4 * 0.32 + u_Time) * 0.1 - 0.4;
    vertexPos.z += wave;
    return vertexPos;
//    vec2 vertexWorldPos = vertexPos.xy + vec2(u_ChunkWorldPos.x, u_ChunkWorldPos.y);
//
//    vec2 origin1 = vertexWorldPos + vec2(dist, dist);
//    vec2 origin2 = vertexWorldPos + vec2(dist, -dist);
//    vec2 origin3 = vertexWorldPos + vec2(-dist, dist);
//    vec2 origin4 = vertexWorldPos + vec2(-dist, -dist);
//
//    float distance1 = length(origin1);
//    float distance2 = length(origin2);
//    float distance3 = length(origin3);
//    float distance4 = length(origin4);
//
//
//    float wave = sin(3.3 * PI * distance1 * 0.13 + u_Time) * 0.1 +
//    sin(3.2 * PI * distance2 * 0.12 + u_Time) * 0.1 +
//    sin(3.1 * PI * distance3 * 0.24 + u_Time) * 0.1 +
//    sin(3.5 * PI * distance4 * 0.32 + u_Time) * 0.1 - 0.4;
//    float waveDist = mix(0.0, wave, (texIndex == 127));
//    vertexPos.z += waveDist;
//    return vertexPos;
}


void main() {
    uint posX = bitfieldExtract(vertexData, 0, 6);
    uint posY = bitfieldExtract(vertexData, 6, 6);
    uint posZ = bitfieldExtract(vertexData, 12, 6);
    uint occlusionLevel = bitfieldExtract(vertexData, 18, 2);
    int x = int(bitfieldExtract(vertexData, 20, 1));
    int y = int(bitfieldExtract(vertexData, 21, 1));
    uint texIndex = bitfieldExtract(vertexData, 22, 8);

    vec3 vertexPos = vec3(posX, posY, posZ);
    vertexPos = applyWave(vertexPos, texIndex);

    v_FragPos = vec3(u_Model * vec4(vertexPos, 1.0));


    gl_Position = u_Projection * u_View * u_Model * vec4(vertexPos, 1.0);
    v_TexCoord = vec3(x, y, texIndex);

    float occlusionFactor = mix(0.0, 1.0, float(u_UseAmbientOcclusion));
    float baseLightLevel = mix(1.0, 0.4, float(u_UseAmbientOcclusion));

    float occlusion = 0.2 * occlusionLevel * occlusionFactor;
    v_LightLevel = baseLightLevel + occlusion;
}