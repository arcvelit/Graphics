#pragma once

#include <vector>
#include <string> 

#include <glm/glm.hpp>  

struct Model 
{
    std::string part_tag;
    std::vector<unsigned int> indices;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int NBO;
    //unsigned int TBO;
    unsigned int IBO;


    glm::vec3 colour;
    int face_count;
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
};

struct Entity
{
    std::vector<Model> entity_parts;
    std::string name;

    std::vector<float> positions;
    std::vector<float> normals;
    //std::vector<float> textCoords;
    
    glm::vec3 middle;
};