#version 330 core
layout (location = 0) in vec2 v_Pos;

uniform mat4 u_Model;

void main()
{
    gl_Position = u_Model * vec4(v_Pos, 0.0, 1.0);
}