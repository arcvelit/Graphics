#include "ModelUtils.h"

#include "Types.h"

#include <iostream>

#include <glm/glm.hpp>  

void modelutils_CenterEntitiesToMiddle(std::vector<Entity> &entities)
{
    for (Entity &entity : entities)
    {
        for (Model &model : entity.entity_parts)
        {
            for (int i = 0; i < model.vertices.size(); i += 6)
            {
                for (int j = 0; j < 3; j++)
                model.vertices[i+j] -= entity.middle[j];
            }
        }
    }
}

void modelutils_SetCameraToStarting(Camera& camera)
{
    camera.position     = glm::vec3(0.0f, 0.0f, 1.5f);
    camera.center       = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.up           = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.modelAngle   = glm::radians(0.0f);
    camera.closeup      = false;
}

void modelutils_SetCameraToZoomOut(Camera& camera)
{
    camera.position = glm::vec3(0.0f, 0.0f, 1.5f);
    camera.closeup  = false;
}

void modelutils_SetCameraToCloseUp(Camera& camera)
{
    camera.position.z   = 0.5f;
    camera.closeup      = true;
}

void modelutils_UpdateCameraLookat(Camera& camera)
{
    camera.center.x = camera.position.x;
    camera.center.y = camera.position.y;
}

void modelutils_NowShowing(Entity* entity)
{
    std::cout << entity->name << "\n";
    std::cout << "Description: " << entity->description << "\n"<< std::endl;
}