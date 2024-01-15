#version 400 core

layout (location = 0) in uint vertexData1;
layout (location = 1) in uint vertexData2;

out vec3 v_FragPos;
out vec3 v_TexCoord;
out vec3 v_FragColor;
flat out uint v_TexIndex;

uniform ivec2 u_ChunkWorldPos;
uniform float u_WorldLightLevel;
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
const float AOcurve[4] = float[4](0.55, 0.75, 0.87, 1.0);
const float SunLightCurve[16] = float[16](
0.000000, 0.066667, 0.133333, 0.200000,
0.266667, 0.333333, 0.400000, 0.466667,
0.533333, 0.600000, 0.666667, 0.733333,
0.800000, 0.866667, 0.933333, 1.000000);
const float TorchLightCurve[16] = float[16](
0.000000, 0.100000, 0.200000, 0.300000,
0.400000, 0.500000, 0.600000, 0.700000,
0.800000, 0.900000, 1.000000, 1.100000,
1.200000, 1.300000, 1.400000, 1.500000);

// only applies for texIndex 8 (water)
vec3 applyWave(vec3 vertexPos, uint texIndex) {
    if (texIndex != 8) {
        return vertexPos;
    }

    if (vertexPos.z < 0.8) return vertexPos;
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
}


void main() {
    uint posX = bitfieldExtract(vertexData1, 0, 6);
    uint posY = bitfieldExtract(vertexData1, 6, 6);
    uint posZ = bitfieldExtract(vertexData1, 12, 6);
    uint occlusionLevel = bitfieldExtract(vertexData1, 18, 2);
    int x = int(bitfieldExtract(vertexData1, 20, 1));
    int y = int(bitfieldExtract(vertexData1, 21, 1));
    uint texIndex = bitfieldExtract(vertexData1, 22, 8);

    // extract lightlevels
    uint blueLightLevel = bitfieldExtract(vertexData2, 0, 4);
    uint greenLightLevel = bitfieldExtract(vertexData2, 4, 4);
    uint redLightLevel = bitfieldExtract(vertexData2, 8, 4);
    uint intensityInt = bitfieldExtract(vertexData2, 12, 4);
    float intensity = max(SunLightCurve[intensityInt] * u_WorldLightLevel, 0.1);
    v_FragColor = vec3(max(TorchLightCurve[redLightLevel], intensity), max(TorchLightCurve[greenLightLevel], intensity), max(TorchLightCurve[blueLightLevel], intensity)) * AOcurve[occlusionLevel];

    vec3 vertexPos = vec3(posX, posY, posZ);
    vertexPos = applyWave(vertexPos, texIndex);

    v_FragPos = vec3(u_Model * vec4(vertexPos, 1.0));

    gl_Position = u_Projection * u_View * u_Model * vec4(vertexPos, 1.0);
    v_TexCoord = vec3(x, y, texIndex);

    float shouldUseOcclusion = mix(0.0, 1.0, float(u_UseAmbientOcclusion));
    float baseLightLevel = mix(1.0, 0.4, float(u_UseAmbientOcclusion));

}