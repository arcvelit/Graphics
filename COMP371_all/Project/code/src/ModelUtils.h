#pragma once

#include "Types.h"

#include <vector>

void modelutils_CenterEntitiesToMiddle(std::vector<Entity> &entities);
void modelutils_SetCameraToStarting(Camera& camera);
void modelutils_SetCameraToZoomOut(Camera& camera);
void modelutils_SetCameraToCloseUp(Camera& camera);
void modelutils_UpdateCameraLookat(Camera& camera);
void modelutils_NowShowing(Entity* entity);