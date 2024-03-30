#version 330
 
layout (location = 0) in vec3 VertexPosition_modelspace;
layout (location = 1) in vec3 VertexNormal_modelspace;

uniform mat4 ModelViewProjection;

void main()
{
    gl_Position = ModelViewProjection * vec4(VertexPosition_modelspace, 1.0);
}