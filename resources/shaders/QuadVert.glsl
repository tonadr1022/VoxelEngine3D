#version 400 core

layout (location = 0) in vec2 vertexPos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in uint colorIndex;

out vec3 v_Color;

uniform mat4 u_MVP;

uniform vec3 colors[6] = vec3[6](
vec3(1.0, 0.0, 0.0), // Red
vec3(1.0, 1.0, 0.0), // Yellow
vec3(1.0, 0.8, 0.0), // Orange
vec3(0.0, 1.0, 0.0), // Green
vec3(0.0, 0.0, 1.0), // Blue
vec3(0.8, 0.0, 1.0)// Pink
);

void main() {
    gl_Position = u_MVP * vec4(vertexPos, 0.0, 1.0);
    v_Color = colors[colorIndex];
}
