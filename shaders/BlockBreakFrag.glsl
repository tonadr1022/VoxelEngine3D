#version 330 core

in vec2 v_TexCoord;

out vec4 o_Color;

uniform sampler2D u_Texture;


void main() {
    //    o_Color = vec4(1.0, 0.0, 0.0, 1.0);

    vec4 texColor = texture(u_Texture, v_TexCoord);

    if(texColor.a < 0.5)
        discard;
//    o_Color = vec4(0.0, 0.0, 0.0, 1.0);
    o_Color = texColor;
}
