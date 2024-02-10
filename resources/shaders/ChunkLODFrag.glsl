#version 400 core

in vec3 v_TexCoord;

out vec4 o_Color;

uniform sampler2DArray u_Texture;
uniform float u_Time;
uniform float u_FirstBufferTime;

void main() {
    vec4 texture = texture(u_Texture, v_TexCoord);
//    if (texture.a < 0.5) {
//        discard;
//    }
//    float chunkLoadingAlphaFactor = clamp((u_Time - u_FirstBufferTime) * 3, 0.0, 1.0);
//    o_Color = vec4(texture.rgb, chunkLoadingAlphaFactor * texture.a);
    o_Color = texture;
    //    o_Color = vec4(v_FragColor, chunkLoadingAlphaFactor * texture.a);
}