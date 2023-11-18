#version 330 core

layout (location=0) in vec3 v_VertexPos;
layout (location=1) in vec2 v_TexCoordIn;
layout (location=2) in vec3 v_VertexNormal;

out vec3 v_FragPos;
out vec2 v_TexCoord;
out vec3 v_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    v_FragPos = vec3(u_Model * vec4(v_VertexPos, 1.0));
    gl_Position = u_Projection * u_View * u_Model * vec4(v_VertexPos, 1.0);
    v_TexCoord = vec2(v_TexCoordIn.x, v_TexCoordIn.y);
    v_Normal = normalize(mat3(transpose(inverse(u_Model))) * v_VertexNormal);
}