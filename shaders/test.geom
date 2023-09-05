#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 mvp;

void main()
{
    int i;
    for (i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
}