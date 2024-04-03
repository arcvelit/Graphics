#version 330 core

in vec3 VertexNormal_worldspace;
in vec3 FragmentPosition_worldspace;

const float GAMMA = 1.0;

uniform vec3 LightPosition_worldspace;
uniform vec3 CameraPosition_worldspace;

uniform float ka;
uniform float kd;
uniform float ks;
uniform float pc;

uniform vec3 ac;
uniform vec3 dc;

uniform int picked;

void main()
{
    vec3 Mat_ac = ac;
    vec3 Mat_dc = dc;

    vec3 N = normalize(VertexNormal_worldspace);
    vec3 L = normalize(LightPosition_worldspace - FragmentPosition_worldspace);
    vec3 V = normalize(CameraPosition_worldspace - FragmentPosition_worldspace);
    vec3 R = normalize(reflect(-L, N));

    float distance = length(LightPosition_worldspace - FragmentPosition_worldspace);

    float cosTheta = clamp(dot(N, L), 0, 1);
	float cosPhi = clamp(dot(R, V), 0, 1);

    float alpha = 1.0;
    float pick = 1.0;
    if (picked == 1)
    {
        Mat_ac = vec3(1.0, 1.0, 0.0);
        Mat_dc = vec3(0.0, 0.0, 0.0);
        pick = 0;
        alpha = 0.8;
    }
    
    vec3 Color = 
    ka * Mat_ac + 
    kd * Mat_dc * cosTheta / (distance*distance)+ 
    ks * pow(cosPhi, pc) * pick / (distance*distance);

    gl_FragColor = vec4(pow(Color, vec3(1.0/GAMMA)), alpha);
}
