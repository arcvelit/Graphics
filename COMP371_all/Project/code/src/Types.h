#pragma once

#include <vector>
#include <string> 

#include <glm/glm.hpp>  

struct Model 
{
    std::string part_tag;
    std::string name;
    std::vector<float> vertices;
    unsigned int VAO;
    unsigned int VBO;

    unsigned int number_of_vertices;

    int face_count;

    float ka;
    float kd;
    float ks;
    float pc;

    glm::vec3 ac;
    glm::vec3 dc;

    std::string description;

    int picked;
};

struct Camera 
{
    glm::vec3 position;
    glm::vec3 center;
    glm::vec3 up;

    float aspectRatio;  
    float nearPlane;  
    float farPlane;  
    float fov;  

    float moveSpeed;
    float rotateSpeed;

    float modelAngle;

    bool closeup;
};

struct Entity
{
    std::vector<Model> entity_parts;
    
    std::string name;
    std::string description;
    
    glm::vec3 middle;
};

struct Light
{
    glm::vec3 position;
};