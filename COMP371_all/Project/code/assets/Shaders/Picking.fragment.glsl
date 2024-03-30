#version 330 core

uniform int selection_code;

void main()
{
    gl_FragColor = vec4(vec3(1.0, 1.0, 1.0) * selection_code / 255.0, 1.0);
}
