#pragma once

#include "Types.h"
#include <string>
#include <vector>

void loadProjectModels(const std::string& directoryPath, std::vector<Entity> &entity_array);
std::string readFile(const std::string& filePath);