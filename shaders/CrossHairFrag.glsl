#version 330 core

out vec4 o_Color;

uniform vec3 u_Color;

void main() {
    o_Color = vec4(u_Color, 1.0);
}