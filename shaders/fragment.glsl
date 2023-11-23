#version 330 core

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 o_Color;

uniform sampler2D u_Texture;

const vec3 sunDirection = normalize(vec3(-1.0, 1.0, 1.0));
const vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main() {
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    float diffuse = max(dot(v_Normal, sunDirection), 0.0);
    vec3 diffuseColor = lightColor * diffuse;

    o_Color = vec4(diffuseColor + ambient, 1.0) * texture(u_Texture, v_TexCoord);
}