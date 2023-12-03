#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 u_Projection;
uniform float u_LineWidth;

void main() {
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;

    vec2 lineDirection = normalize(p1.xy - p0.xy);
    vec2 normal = vec2(-lineDirection.y, lineDirection.x);

    vec4 offset = vec4(normal * u_LineWidth / 2.0, 0.0, 0.0);

    gl_Position = u_Projection * (p0 + offset);
    EmitVertex();

    gl_Position = u_Projection * (p0 - offset);
    EmitVertex();

    gl_Position = u_Projection * (p1 + offset);
    EmitVertex();

    gl_Position = u_Projection * (p1 - offset);
    EmitVertex();

    EndPrimitive();

}