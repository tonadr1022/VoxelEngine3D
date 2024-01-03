#version 400 core

in vec3 v_TexCoord;
in float v_LightLevel;
in vec3 v_FragPos;
flat in uint v_TexIndex;

out vec4 o_Color;

uniform sampler2DArray u_Texture;
uniform float u_Time;
uniform float u_FirstBufferTime;

const vec3 sunDirection = normalize(vec3(-1.0, 1.0, 1.0));
const vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main() {
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    vec4 texture = texture(u_Texture, v_TexCoord);
    float chunkLoadingAlphaFactor = clamp((u_Time - u_FirstBufferTime) * 3, 0.0, 1.0);
    o_Color = vec4(texture.rgb * v_LightLevel, chunkLoadingAlphaFactor * texture.a);
//        o_Color = vec4(v_LightLevel, v_LightLevel, v_LightLevel, 1.0);
}