#version 330 core

in vec2 v_TexCoord;

out vec4 o_Color;

uniform sampler2DArray u_Texture;
uniform int u_TexIndex;

void main() {

    vec4 texColor = texture(u_Texture, vec3(v_TexCoord, u_TexIndex));

    if(texColor.a == 0.0)
        discard;
    o_Color = texColor;
}
