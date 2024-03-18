#include "ModelUtils.h"

#include "Types.h"

#include <glm/glm.hpp>  

void modelutils_SetCameraToStarting(Camera& camera)
{
    camera.position     = glm::vec3(0.0f, 0.0f, 2.0f);
    camera.center       = glm::vec3(0.0f, camera.position.y, 0.0f);
    camera.up           = glm::vec3(0.0f, 1.0f, 0.0f);
    camera.modelAngle   = glm::radians(0.0f);
}

void modelutils_UpdateCameraLookat(Camera& camera)
{
    camera.center.x = camera.position.x;
    camera.center.y = camera.position.y;
}