#version 400 core

in vec3 v_Color;

out vec4 o_Color;

uniform sampler2DArray u_Texture;



void main() {
    o_Color = vec4(v_Color, 1.0);
}