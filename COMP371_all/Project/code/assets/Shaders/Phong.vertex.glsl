#version 330 core

layout (location = 0) in vec3 VertexPosition_modelspace;
layout (location = 1) in vec3 VertexNormal_modelspace;

uniform mat4 ModelViewProjection, Model;

// Interpolated stuff
out vec3 VertexNormal_worldspace;
out vec3 FragmentPosition_worldspace;

void main()
{
    gl_Position = ModelViewProjection * vec4(VertexPosition_modelspace, 1.0);

    VertexNormal_worldspace = transpose(inverse(mat3(Model))) * VertexNormal_modelspace;
    FragmentPosition_worldspace = mat3(Model) * VertexPosition_modelspace;
}

